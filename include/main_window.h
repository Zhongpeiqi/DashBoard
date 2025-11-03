#pragma once
#include <QMainWindow>
#include <QComboBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include "dashboard.h"
#include "wave_config_dialog.h"
#include "backend/waveconfig.h"

/**
 * @brief MainWindow —— 主程序窗口
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

    // Dashboard 数据更新接口
    void updateWind(double direction, double speed, double roll);
    void updateNav(double lat, double lon, double alt,
                   double cog, double sog, double pitch);
    void updateEngines(double portOrder, double portRudder,
                       double stbdOrder, double stbdRudder);
    void updatePropulsion(double bowThrust, double mainSpeed, double sternThrust);
    void updateSpeedAndRudder(double accel, double mainSpeed,
                              double rudder_port, double rudder_stbd);

signals:
    // 对外信号 —— 发给 MQTT 模块
    void sendWaveConfigToMqtt(const WaveConfig& cfg);   // 风浪参数更新
    void sendBoatSelectionToMqtt(const QString& boatName); // 当前船只选择变化
    // 新增：控制状态信号
    void sendControlStatusToMqtt(const QString& status); // 控制状态更新

public slots:
    void onBoatChanged(const QString& name);          // 当切换船只时触发
    void onOpenWaveConfig();                          // 打开波浪参数配置对话框
    void onWaveConfigChanged(const WaveConfig& cfg);  // 接收配置变化信号
    void onWaveConfigPublished(const WaveConfig& config);
    // 新增：控制按钮槽函数
    void onAutoClicked();
    void onManualClicked();
    void onStopClicked();

private:
    QString currentBoat_;         // 当前选中船只
    WaveConfig currentWaveConfig_; // 当前波浪参数配置
    QString currentControlStatus_; // 当前控制状态

    QComboBox* boatSelector_;     // 下拉框组件
    QPushButton* waveConfigBtn_;  // 波浪配置对话框按钮
    // 新增：控制按钮
    QPushButton* autoBtn_;        // AUTO按钮
    QPushButton* manualBtn_;      // MANUAL按钮
    QPushButton* stopBtn_;        // STOP按钮

    DashBoard* dashboard_;        // 仪表盘组件
};
