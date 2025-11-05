#ifndef VIDEO_PLAYER_WIDGET_H
#define VIDEO_PLAYER_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "backend/ffmpegdecoder.h"

class VideoPlayerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoPlayerWidget(QWidget *parent = nullptr);
    ~VideoPlayerWidget();

    // 根据当前选择的船只自动切换摄像头
    void setCurrentBoat(const QString &boatName);

private slots:
    void onPlayClicked();
    void onStopClicked();
    void onCameraPresetChanged(int index);
    void onFrameReady(const QImage &frame);
    void onStateChanged(int state);
    void onErrorOccurred(const QString &errorMessage);

private:
    void setupUI();
    void setupConnections();
    void resizeEvent(QResizeEvent *event) override;
    void playSelectedCamera(); // 播放选中的摄像头

    FFmpegDecoder *m_decoder;
    QLabel *m_videoLabel;
    QLabel *m_statusLabel;
    QPushButton *m_playButton;
    QPushButton *m_stopButton;
    QLineEdit *m_urlEdit;
    QComboBox *m_cameraPresets;

    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_controlLayout;

    QSize m_lastFrameSize;
    QPixmap m_cachedPixmap;

    QString m_currentCameraUrl; // 记录当前播放的摄像头URL
};

#endif // VIDEO_PLAYER_WIDGET_H

