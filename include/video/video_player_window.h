#ifndef VIDEO_PLAYER_WINDOW_H
#define VIDEO_PLAYER_WINDOW_H

#include <QMainWindow>
#include "video/video_player_widget.h"

class VideoPlayerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit VideoPlayerWindow(QWidget *parent = nullptr);
    ~VideoPlayerWindow();

    // 根据当前选择的船只自动切换摄像头
    void setCurrentBoat(const QString &boatName);

protected:
    void closeEvent(QCloseEvent *event) override;

signals:
    void windowClosed(); // 窗口关闭信号

private:
    VideoPlayerWidget *m_videoPlayerWidget;
};

#endif // VIDEO_PLAYER_WINDOW_H

