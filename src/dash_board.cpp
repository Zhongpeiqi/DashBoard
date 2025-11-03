#include "dashboard.h"
#include "wind_roll/wind_gauge.h"
#include "nav_info/nav_info_widget.h"
#include "engine/port_engine_container.h"
#include "engine/stbd_engine_container.h"
#include "propulsion/propulsion_panel_widget.h"
#include <QFrame>
#include <QHBoxLayout>
#include <QDebug>

/**
 * @brief DashBoard 构造函数
 *
 * 1️⃣ 初始化所有子控件；
 * 2️⃣ 建立水平布局；
 * 3️⃣ 设置比例（4 : 3 : 3 : 3 : 3）；
 * 4️⃣ 背景黑色统一风格。
 */
DashBoard::DashBoard(QWidget* parent)
    : QWidget(parent)
{
     setMinimumSize(1500,800);
    // 背景灰色
    setStyleSheet("background-color: balck;");

    // ======== 初始化子模块 ========
    wind_       = new WindGauge(this);
    navInfo_    = new NavInfoWidget(this);
    portEngine_ = new PortEngineContainer(this);
    propulsion_ = new PropulsionPanelWidget(this);
    stbdEngine_ = new StbdEngineContainer(this);

    // ======== 主布局：水平排列 ========
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 0, 10, 0);
    // layout->setSpacing(20);

    // 设置比例，并插入分隔线
    layout->addWidget(wind_, 3);
    layout->addWidget(createDivider(4, "#888888"));  // 左 WIND 与导航之间：普通浅灰

    layout->addWidget(navInfo_, 3);
    layout->addWidget(createDivider(8, "#888888"));  // 中央导航与PORT：稍粗中灰

    layout->addWidget(portEngine_, 3);
    layout->addWidget(createDivider(4, "#888888"));  // PORT 与推进之间

    layout->addWidget(propulsion_, 3);
    layout->addWidget(createDivider(4, "#888888"));  // 推进与STBD之间

    layout->addWidget(stbdEngine_, 3);

    setLayout(layout);
}

/**
 * @brief 设置风仪表数据
 */
void DashBoard::setWind(double dir, double spd, double roll)
{
    if (wind_) {
        wind_->setDirection(dir);
        wind_->setSpeed(spd);
        wind_->setRoll(roll);
    }
}

/**
 * @brief 设置导航信息（经纬度、高度、航向、航速、俯仰）
 */
void DashBoard::setNav(double lat, double lon, double alt, double cog, double sog, double pitch)
{
    if (navInfo_) {
        navInfo_->setLat(lat);
        navInfo_->setLon(lon);
        navInfo_->setAlt(alt);
        navInfo_->setCog(cog);
        navInfo_->setSog(sog);
        navInfo_->setPitch(pitch);
    }
}

/**
 * @brief 同时更新左右引擎的推力与舵角
 */
void DashBoard::setEngines(double portOrder, double portRudder, double stbdOrder, double stbdRudder)
{
    if (portEngine_) portEngine_->setData(portOrder, portRudder);
    if (stbdEngine_) stbdEngine_->setData(stbdOrder, stbdRudder);
}

/**
 * @brief 更新推进系统（艏推、速度、艉推）
 */
void DashBoard::setPropulsion(double bowThrust, double mainSpeed, double sternThrust)
{
    if (propulsion_) propulsion_->setValues(bowThrust, mainSpeed, sternThrust);
}

/**
 * @brief 控制 SpeedLogWidget 内的上下/左右绿色三角
 *
 * - accel 正 → 上三角绿；负 → 下三角绿；
 * - rudder_port 正 → 左侧第1个右三角绿；负 → 左三角绿；
 * - rudder_stbd 正 → 右侧第2个右三角绿；负 → 左三角绿；
 */
void DashBoard::setSpeedAndRudder(double accel, double mainSpeed, double rudder_port, double rudder_stbd)
{
    if (propulsion_) {
        propulsion_->setSpeedInfo(accel, mainSpeed);
        propulsion_->setRudderAngles(rudder_port, rudder_stbd);
    }
}
//绘制细线分割组件
QFrame* DashBoard::createDivider(int width, const QString& color)
{
    QFrame* line = new QFrame;
    line->setFixedWidth(width);  // 线的粗细
    line->setStyleSheet(QString("background-color: %1;").arg(color));  // 实心色
    return line;
}

