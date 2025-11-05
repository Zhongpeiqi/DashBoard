#include "backend/ffmpegdecoder.h"
#include <QDebug>
#include <QtConcurrent>
#include <QThread>

FFmpegDecoder::FFmpegDecoder(QObject *parent) : QObject(parent)
{
    initFFmpeg();
}

FFmpegDecoder::~FFmpegDecoder()
{
    stop();
    cleanup();
}

bool FFmpegDecoder::initFFmpeg()
{
    QMutexLocker locker(&m_mutex);

    // 初始化FFmpeg网络模块
    avformat_network_init();
    qDebug() << "FFmpeg network initialized";

    return true;
}

bool FFmpegDecoder::openStream(const QString &url)
{
    QMutexLocker locker(&m_mutex);

    if (m_state == Playing || m_state == Connecting) {
        m_errorString = "Stream is already open";
        return false;
    }

    setState(Connecting);
    cleanup();

    // 优化RTSP选项
        AVDictionary *options = nullptr;
        av_dict_set(&options, "rtsp_transport", "tcp", 0);
        av_dict_set(&options, "stimeout", "3000000", 0);    // 3秒超时
        av_dict_set(&options, "max_delay", "100000", 0);    // 减少延迟
        av_dict_set(&options, "fflags", "nobuffer", 0);     // 无缓冲
        av_dict_set(&options, "flags", "low_delay", 0);     // 低延迟模式
        av_dict_set(&options, "analyzeduration", "1000000", 0); // 减少分析时间

        // 对于实时流，跳过帧以提高响应速度
        av_dict_set(&options, "skip_frame", "default", 0);

    // 打开输入流
    int ret = avformat_open_input(&m_formatContext, url.toUtf8().constData(), nullptr, &options);
    av_dict_free(&options);

    if (ret != 0) {
        m_errorString = QString("Failed to open stream: %1").arg(ret);
        setState(Error);
        return false;
    }

    // 获取流信息
    if (avformat_find_stream_info(m_formatContext, nullptr) < 0) {
        m_errorString = "Failed to retrieve stream information";
        setState(Error);
        return false;
    }

    // 查找视频流
    m_videoStreamIndex = -1;
    for (unsigned int i = 0; i < m_formatContext->nb_streams; i++) {
        if (m_formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            m_videoStreamIndex = i;
            break;
        }
    }

    if (m_videoStreamIndex == -1) {
        m_errorString = "No video stream found";
        setState(Error);
        return false;
    }

    // 初始化解码器
    if (!initCodec()) {
        return false;
    }

    // 初始化图像转换上下文
    if (!initSwsContext()) {
        return false;
    }

    m_stopped = false;
    m_paused = false;

    // 在单独的线程中开始解码
    // 正确的Qt6写法
    QtConcurrent::run(&FFmpegDecoder::run, this);

    return true;
}

bool FFmpegDecoder::initCodec()
{
    AVCodecParameters *codecParams = m_formatContext->streams[m_videoStreamIndex]->codecpar;
    const AVCodec *codec = avcodec_find_decoder(codecParams->codec_id);

    if (!codec) {
        m_errorString = "Unsupported codec";
        setState(Error);
        return false;
    }

    m_codecContext = avcodec_alloc_context3(codec);
    if (!m_codecContext) {
        m_errorString = "Failed to allocate codec context";
        setState(Error);
        return false;
    }

    if (avcodec_parameters_to_context(m_codecContext, codecParams) < 0) {
        m_errorString = "Failed to copy codec parameters";
        setState(Error);
        return false;
    }

    // 优化解码器参数
        m_codecContext->flags2 |= AV_CODEC_FLAG2_FAST;    // 启用快速解码
        m_codecContext->thread_count = 4;                // 多线程解码
        m_codecContext->thread_type = FF_THREAD_FRAME;   // 帧级多线程

        // 设置低延迟参数
        m_codecContext->flags |= AV_CODEC_FLAG_LOW_DELAY;

    if (avcodec_open2(m_codecContext, codec, nullptr) < 0) {
        m_errorString = "Failed to open codec";
        setState(Error);
        return false;
    }

    m_videoWidth = m_codecContext->width;
    m_videoHeight = m_codecContext->height;

    return true;
}

bool FFmpegDecoder::initSwsContext()
{
    m_frame = av_frame_alloc();
    m_frameRGB = av_frame_alloc();
    m_packet = av_packet_alloc();

    if (!m_frame || !m_frameRGB || !m_packet) {
        m_errorString = "Failed to allocate frames/packet";
        setState(Error);
        return false;
    }

    // 使用更快的缩放算法
        m_swsContext = sws_getContext(m_videoWidth, m_videoHeight, m_codecContext->pix_fmt,
                                     m_videoWidth, m_videoHeight, AV_PIX_FMT_RGB32,
                                     SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);

    if (!m_swsContext) {
        m_errorString = "Failed to create image conversion context";
        setState(Error);
        return false;
    }

    // 分配RGB缓冲区
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB32, m_videoWidth, m_videoHeight, 1);
    m_rgbBuffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));

    av_image_fill_arrays(m_frameRGB->data, m_frameRGB->linesize, m_rgbBuffer,
                        AV_PIX_FMT_RGB32, m_videoWidth, m_videoHeight, 1);

    return true;
}

void FFmpegDecoder::run()
{
    // 设置高优先级
    QThread::currentThread()->setPriority(QThread::HighPriority);
    setState(Playing);

    // 使用更积极的数据包读取策略
    while (!m_stopped) {
        if (m_paused) {
            QMutexLocker locker(&m_mutex);
            m_pauseCondition.wait(&m_mutex);
            if (m_stopped) break;
        }

        // 连续读取多个包，减少锁竞争
        for (int i = 0; i < 5 && !m_stopped; i++) {
            int ret = av_read_frame(m_formatContext, m_packet);
            if (ret < 0) {
                if (ret == AVERROR_EOF) {
                    qDebug() << "End of stream";
                    break;
                }
                // 短暂休眠后继续尝试
                QThread::msleep(1);
                continue;
            }

            if (m_packet->stream_index == m_videoStreamIndex) {
                if (!decodePacket(m_packet)) {
                    av_packet_unref(m_packet);
                    break;
                }
            }
            av_packet_unref(m_packet);
        }
    }

    setState(Stopped);
}

bool FFmpegDecoder::decodePacket(AVPacket *packet)
{
    int ret = avcodec_send_packet(m_codecContext, packet);
    if (ret < 0) {
        qDebug() << "Error sending packet to decoder";
        return true; // 继续处理下一个包
    }

    while (ret >= 0) {
        ret = avcodec_receive_frame(m_codecContext, m_frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return true;
        } else if (ret < 0) {
            qDebug() << "Error receiving frame from decoder";
            return true;
        }

        // 转换帧格式 (YUV to RGB)
        sws_scale(m_swsContext,
                 (uint8_t const * const *)m_frame->data, m_frame->linesize,
                 0, m_videoHeight,
                 m_frameRGB->data, m_frameRGB->linesize);

        // 创建QImage并发送信号
        QImage image(m_frameRGB->data[0], m_videoWidth, m_videoHeight,
                    m_frameRGB->linesize[0], QImage::Format_RGB32);

        emit frameReady(image.copy()); // 复制图像数据以确保线程安全

        av_frame_unref(m_frame);
    }

    return true;
}

void FFmpegDecoder::pause()
{
    m_paused = true;
    setState(Paused);
}

void FFmpegDecoder::resume()
{
    m_paused = false;
    m_pauseCondition.wakeAll();
    setState(Playing);
}

void FFmpegDecoder::stop()
{
    m_stopped = true;
    m_paused = false;
    m_pauseCondition.wakeAll();
    setState(Stopped);
}

void FFmpegDecoder::closeStream()
{
    stop();
    cleanup();
}

void FFmpegDecoder::setState(DecoderState newState)
{
    if (m_state != newState) {
        m_state = newState;
        emit stateChanged(static_cast<int>(newState));
    }
}

void FFmpegDecoder::cleanup()
{
    if (m_rgbBuffer) {
        av_free(m_rgbBuffer);
        m_rgbBuffer = nullptr;
    }

    if (m_swsContext) {
        sws_freeContext(m_swsContext);
        m_swsContext = nullptr;
    }

    if (m_frame) {
        av_frame_free(&m_frame);
        m_frame = nullptr;
    }

    if (m_frameRGB) {
        av_frame_free(&m_frameRGB);
        m_frameRGB = nullptr;
    }

    if (m_packet) {
        av_packet_free(&m_packet);
        m_packet = nullptr;
    }

    if (m_codecContext) {
        avcodec_free_context(&m_codecContext);
        m_codecContext = nullptr;
    }

    if (m_formatContext) {
        avformat_close_input(&m_formatContext);
        m_formatContext = nullptr;
    }

    m_videoStreamIndex = -1;
    m_videoWidth = 0;
    m_videoHeight = 0;
}

