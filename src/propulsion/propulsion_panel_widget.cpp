#include "propulsion/propulsion_panel_widget.h"
#include "propulsion/thruster_gauge_widget.h"
#include "propulsion/speed_log_widget.h"
#include <QVBoxLayout>

/**
 * @brief 构造函数 —— 初始化推进系统布局
 */
PropulsionPanelWidget::PropulsionPanelWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumWidth(250);
    // 背景
    setAutoFillBackground(true);
    setStyleSheet("background-color: black;");

    // 主布局
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(6, 6, 6, 6);
    layout->setSpacing(10);

    // ===== 顶部艏推 =====
    thruster_bow_ = new ThrusterGaugeWidget(this);
    thruster_bow_->setMinimumHeight(120);
    thruster_bow_->setLabel("BOW");
    layout->addWidget(thruster_bow_, 1);

    // ===== 中间 Speed Log =====
    speed_log_ = new SpeedLogWidget(this);
    speed_log_->setMinimumHeight(240);
    layout->addWidget(speed_log_, 2);

    // ===== 底部艉推 =====
    thruster_stern_ = new ThrusterGaugeWidget(this);
    thruster_stern_->setMinimumHeight(120);
    thruster_stern_->setLabel("STERN");
    layout->addWidget(thruster_stern_, 1);

    setLayout(layout);
}

/**
 * @brief 设置基本推进系统数值（艏推、主速、艉推）
 */
void PropulsionPanelWidget::setValues(double bowThrust, double mainSpeed, double sternThrust)
{
    thruster_bow_->setThrust(bowThrust);
    if (speed_log_) speed_log_->setSpeed(bowThrust, mainSpeed, sternThrust);
    thruster_stern_->setThrust(sternThrust);
}

/**
 * @brief 新增接口：设置中间 SpeedLog 的上下三角与主速
 * @param accel 上下三角加速度
 * @param speed 当前航速
 */
void PropulsionPanelWidget::setSpeedInfo(double accel, double speed)
{
    if (!speed_log_) return;
    speed_log_->setSpeed(accel, speed, accel);  // 上下同一个加速度，中间为速度
}

/**
 * @brief 新增接口：设置左右舵角状态（控制左右绿色三角）
 * @param rudder_port 左舵角
 * @param rudder_stbd 右舵角
 */
void PropulsionPanelWidget::setRudderAngles(double rudder_port, double rudder_stbd)
{
    if (!speed_log_) return;
    speed_log_->setRudders(rudder_port, rudder_stbd);
}
