#ifndef FFMPEGDECODER_H
#define FFMPEGDECODER_H

#include <QObject>
#include <QImage>
#include <QMutex>
#include <QWaitCondition>

extern "C" {
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswscale/swscale.h>
    #include <libavutil/imgutils.h>
}

class FFmpegDecoder : public QObject
{
    Q_OBJECT

public:
    enum DecoderState {
        Stopped,
        Connecting,
        Playing,
        Paused,
        Error
    };

    explicit FFmpegDecoder(QObject *parent = nullptr);
    ~FFmpegDecoder();

    bool openStream(const QString &url);
    void closeStream();
    void pause();
    void resume();
    void stop();

    DecoderState getState() const { return m_state; }
    QString getErrorString() const { return m_errorString; }
    int getVideoWidth() const { return m_videoWidth; }
    int getVideoHeight() const { return m_videoHeight; }

signals:
    void frameReady(const QImage &frame);
    void stateChanged(int state);
    void errorOccurred(const QString &errorMessage);

private:
    void setState(DecoderState newState);
    bool initFFmpeg();
    bool initCodec();
    bool initSwsContext();
    bool decodePacket(AVPacket *packet);
    void cleanup();
    void run();

    // FFmpeg相关变量
    AVFormatContext *m_formatContext = nullptr;
    AVCodecContext *m_codecContext = nullptr;
    AVFrame *m_frame = nullptr;
    AVFrame *m_frameRGB = nullptr;
    AVPacket *m_packet = nullptr;
    SwsContext *m_swsContext = nullptr;

    int m_videoStreamIndex = -1;
    uint8_t *m_rgbBuffer = nullptr;

    // 视频信息
    int m_videoWidth = 0;
    int m_videoHeight = 0;

    // 状态控制
    volatile bool m_stopped = false;
    volatile bool m_paused = false;
    DecoderState m_state = Stopped;
    QString m_errorString;

    QMutex m_mutex;
    QWaitCondition m_pauseCondition;
};

#endif // FFMPEGDECODER_H

