#pragma once
#include <QWidget>

class ThrusterGaugeWidget;
class SpeedLogWidget;

/**
 * @brief PropulsionPanelWidget —— 推进系统主容器
 */
class PropulsionPanelWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PropulsionPanelWidget(QWidget* parent = nullptr);

    // 更新艏推、主速、艉推的推力
    void setValues(double bowThrust, double mainSpeed, double sternThrust);

    void setSpeedInfo(double accel, double speed);                  // 控制中间 SpeedLog 的上下三角与速度
    void setRudderAngles(double rudder_port, double rudder_stbd);   // 控制左右绿三角

private:
    ThrusterGaugeWidget* thruster_bow_   = nullptr;  // 艏推
    ThrusterGaugeWidget* thruster_stern_ = nullptr;  // 艉推
    SpeedLogWidget*      speed_log_      = nullptr;  // 船速显示（LOG 区）
};
