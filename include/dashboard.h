#pragma once
#include <QWidget>
#include <QFrame>

class WindGauge;
class NavInfoWidget;
class PortEngineContainer;
class StbdEngineContainer;
class PropulsionPanelWidget;

/**
 * @brief DashBoard —— 船舶综合仪表面板
 *
 * 封装全部仪表：
 *  ├── 风仪表 (WindGauge)
 *  ├── 导航信息 (NavInfoWidget)
 *  ├── 左舷引擎 (PortEngineContainer)
 *  ├── 推进系统 (PropulsionPanelWidget, 内含 SpeedLog)
 *  └── 右舷引擎 (StbdEngineContainer)
 *
 * 提供统一 set 接口更新所有子模块。
 */
class DashBoard : public QWidget
{
    Q_OBJECT
public:
    explicit DashBoard(QWidget* parent = nullptr);

    // ========== 数据更新接口 ==========
    void setWind(double dir, double spd, double roll);
    void setNav(double lat, double lon, double alt, double cog, double sog, double pitch);
    void setEngines(double portOrder, double portRudder, double stbdOrder, double stbdRudder);
    void setPropulsion(double bowThrust, double mainSpeed, double sternThrust);
    void setSpeedAndRudder(double accel, double mainSpeed, double rudder_port, double rudder_stbd);

private:
    WindGauge*             wind_       = nullptr;
    NavInfoWidget*         navInfo_    = nullptr;
    PortEngineContainer*   portEngine_ = nullptr;
    PropulsionPanelWidget* propulsion_ = nullptr;
    StbdEngineContainer*   stbdEngine_ = nullptr;
    QFrame* createDivider(int width = 2, const QString& color = "#444444");

};
