#include "video/video_player_window.h"
#include <QVBoxLayout>
#include <QCloseEvent>

VideoPlayerWindow::VideoPlayerWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("船舶摄像头监控系统");
    resize(800, 600);
    setMinimumSize(640, 480);

    // 创建视频播放器组件
    m_videoPlayerWidget = new VideoPlayerWidget(this);
    setCentralWidget(m_videoPlayerWidget);
}

VideoPlayerWindow::~VideoPlayerWindow()
{
    // VideoPlayerWidget会自动清理
}

void VideoPlayerWindow::setCurrentBoat(const QString &boatName)
{
    if (m_videoPlayerWidget) {
        m_videoPlayerWidget->setCurrentBoat(boatName);
    }
}

void VideoPlayerWindow::closeEvent(QCloseEvent *event)
{
    emit windowClosed();
    event->accept();
}

