#include "video/video_player_widget.h"
#include <QApplication>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>
#include <QResizeEvent>

VideoPlayerWidget::VideoPlayerWidget(QWidget *parent)
    : QWidget(parent), m_decoder(new FFmpegDecoder(this)), m_currentCameraUrl("")
{
    setupUI();
    setupConnections();

    // 设置窗口属性
    setMinimumSize(640, 480);

    // 添加预设摄像头URL
    m_cameraPresets->addItem("选择船只摄像头...");
    m_cameraPresets->addItem("boat1", "rtsp://60.205.13.156:8554/vrx_boat1");
    m_cameraPresets->addItem("boat2", "rtsp://60.205.13.156:8554/vrx_boat2");
    m_cameraPresets->addItem("boat3", "rtsp://60.205.13.156:8554/vrx_boat3");
}

VideoPlayerWidget::~VideoPlayerWidget()
{
    m_decoder->stop();
}

void VideoPlayerWidget::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(5, 5, 5, 5);
    m_mainLayout->setSpacing(5);

    // 视频显示区域 - 设置为可扩展
    m_videoLabel = new QLabel("视频将在这里显示");
    m_videoLabel->setAlignment(Qt::AlignCenter);
    m_videoLabel->setStyleSheet("QLabel { background-color: black; color: white; }");
    m_videoLabel->setMinimumSize(640, 480);
    m_videoLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_mainLayout->addWidget(m_videoLabel, 1);

    // 状态显示
    m_statusLabel = new QLabel("状态: 就绪");
    m_statusLabel->setStyleSheet("color: white; background-color: #333; padding: 5px;");
    m_mainLayout->addWidget(m_statusLabel);

    // 控制区域
    m_controlLayout = new QHBoxLayout();

    // 摄像头选择
    m_cameraPresets = new QComboBox();
    m_controlLayout->addWidget(new QLabel("船只摄像头:"));
    m_controlLayout->addWidget(m_cameraPresets);

    // URL输入
    m_urlEdit = new QLineEdit();
    m_urlEdit->setPlaceholderText("输入RTSP流地址 (如: rtsp://username:password@ip:port/stream)");
    m_urlEdit->setMinimumWidth(300);
    m_controlLayout->addWidget(new QLabel("RTSP URL:"));
    m_controlLayout->addWidget(m_urlEdit);

    // 按钮
    m_playButton = new QPushButton("播放");
    m_stopButton = new QPushButton("停止");
    m_stopButton->setEnabled(false);

    m_controlLayout->addWidget(m_playButton);
    m_controlLayout->addWidget(m_stopButton);

    m_mainLayout->addLayout(m_controlLayout);
}

void VideoPlayerWidget::setupConnections()
{
    connect(m_playButton, &QPushButton::clicked, this, &VideoPlayerWidget::onPlayClicked);
    connect(m_stopButton, &QPushButton::clicked, this, &VideoPlayerWidget::onStopClicked);

    // 修改摄像头选择信号连接
    connect(m_cameraPresets, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &VideoPlayerWidget::onCameraPresetChanged);

    connect(m_decoder, &FFmpegDecoder::frameReady, this, &VideoPlayerWidget::onFrameReady);
    connect(m_decoder, &FFmpegDecoder::stateChanged, this, &VideoPlayerWidget::onStateChanged);
    connect(m_decoder, &FFmpegDecoder::errorOccurred, this, &VideoPlayerWidget::onErrorOccurred);
}

void VideoPlayerWidget::setCurrentBoat(const QString &boatName)
{
    // 根据船只名称选择对应的摄像头
    int index = -1;
    if (boatName == "boat1") {
        index = 1;
    } else if (boatName == "boat2") {
        index = 2;
    } else if (boatName == "boat3") {
        index = 3;
    }

    if (index > 0 && index < m_cameraPresets->count()) {
        m_cameraPresets->setCurrentIndex(index);
        // onCameraPresetChanged 会自动触发播放
    }
}

void VideoPlayerWidget::onCameraPresetChanged(int index)
{
    if (index > 0) {
        QString url = m_cameraPresets->currentData().toString();
        if (!url.isEmpty()) {
            m_urlEdit->setText(url);
            // 自动播放选中的摄像头
            playSelectedCamera();
        }
    } else {
        // 选择了"选择船只摄像头..."，停止播放
        onStopClicked();
    }
}

void VideoPlayerWidget::playSelectedCamera()
{
    QString url = m_urlEdit->text().trimmed();
    if (url.isEmpty()) {
        return;
    }

    // 如果当前正在播放其他摄像头，先停止
    if (m_decoder->getState() == FFmpegDecoder::Playing && url != m_currentCameraUrl) {
        m_decoder->stop();
        // 清空视频显示
        m_videoLabel->setText("正在切换摄像头...");
        m_cachedPixmap = QPixmap();

        // 使用单次定时器，确保停止操作完成后再开始新的播放
        QTimer::singleShot(100, [this, url]() {
            if (m_decoder->openStream(url)) {
                m_currentCameraUrl = url;
                m_playButton->setEnabled(false);
                m_stopButton->setEnabled(true);
                m_statusLabel->setText("状态: 切换摄像头中...");
            }
        });
    } else if (m_decoder->getState() != FFmpegDecoder::Playing) {
        // 当前没有播放，直接开始
        if (m_decoder->openStream(url)) {
            m_currentCameraUrl = url;
            m_playButton->setEnabled(false);
            m_stopButton->setEnabled(true);
            m_statusLabel->setText("状态: 连接中...");
        }
    }
    // 如果选择的是同一个摄像头，不执行任何操作
}

void VideoPlayerWidget::onPlayClicked()
{
    QString url = m_urlEdit->text().trimmed();
    if (url.isEmpty()) {
        QMessageBox::warning(this, "错误", "请输入有效的RTSP URL");
        return;
    }

    if (m_decoder->openStream(url)) {
        m_currentCameraUrl = url;
        m_playButton->setEnabled(false);
        m_stopButton->setEnabled(true);
        m_statusLabel->setText("状态: 连接中...");
    }
}

void VideoPlayerWidget::onStopClicked()
{
    m_decoder->stop();
    m_playButton->setEnabled(true);
    m_stopButton->setEnabled(false);
    m_statusLabel->setText("状态: 已停止");
    m_currentCameraUrl = "";

    // 清空视频显示
    m_videoLabel->setText("视频将在这里显示");
    m_cachedPixmap = QPixmap();
}

void VideoPlayerWidget::onFrameReady(const QImage &frame)
{
    // 如果帧大小改变或窗口大小改变，重新计算缩放
    if (m_lastFrameSize != frame.size() || m_cachedPixmap.isNull()) {
        m_lastFrameSize = frame.size();

        // 计算保持宽高比的缩放
        QSize labelSize = m_videoLabel->size();
        QPixmap pixmap = QPixmap::fromImage(frame);

        // 计算缩放后的大小，保持宽高比
        QSize scaledSize = frame.size();
        scaledSize.scale(labelSize, Qt::KeepAspectRatio);

        m_cachedPixmap = pixmap.scaled(scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    } else {
        // 直接使用缓存的缩放设置，只更新图像内容
        QPixmap tempPixmap = QPixmap::fromImage(frame);
        m_cachedPixmap = tempPixmap.scaled(m_cachedPixmap.size(),
                                         Qt::IgnoreAspectRatio,
                                         Qt::SmoothTransformation);
    }

    m_videoLabel->setPixmap(m_cachedPixmap);
}

void VideoPlayerWidget::onStateChanged(int state)
{
    QString stateText;
    switch (state) {
    case FFmpegDecoder::Stopped:
        stateText = "已停止";
        m_playButton->setEnabled(true);
        m_stopButton->setEnabled(false);
        break;
    case FFmpegDecoder::Connecting:
        stateText = "连接中...";
        break;
    case FFmpegDecoder::Playing:
        stateText = "播放中";
        break;
    case FFmpegDecoder::Error:
        stateText = "错误";
        m_playButton->setEnabled(true);
        m_stopButton->setEnabled(false);
        m_currentCameraUrl = ""; // 出错时清空当前URL
        break;
    }

    m_statusLabel->setText(QString("状态: %1").arg(stateText));
}

void VideoPlayerWidget::onErrorOccurred(const QString &errorMessage)
{
    QMessageBox::critical(this, "错误", errorMessage);
    m_statusLabel->setText("状态: 错误 - " + errorMessage);
    m_currentCameraUrl = ""; // 出错时清空当前URL
}

void VideoPlayerWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    // 窗口大小改变时，清空缓存，强制重新计算缩放
    m_cachedPixmap = QPixmap();
}

