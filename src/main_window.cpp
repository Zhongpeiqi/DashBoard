#include "main_window.h"
#include <QVBoxLayout>
#include <QStatusBar>
#include <QDebug>
#include <QLabel>
#include <QMessageBox>
#include "backend/waveconfig.h"

/**
 * @brief MainWindow 构造函数
 */
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , currentWaveConfig_{1.0, 5.0, 0.0, 0.1, 1.0}  // 初始化默认值
{
    setWindowTitle("Ship Dashboard Control Center");
    resize(1600, 900);

    // ========== 顶部控制栏 ==========
    QWidget* topBar = new QWidget(this);
    auto* topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(10, 5, 10, 5);  // 左右10px，上下5px间距
    topLayout->setSpacing(20);

    QLabel* boatLabel = new QLabel("当前控制船只:", topBar);
    boatLabel->setStyleSheet("color: black; font-size: 15px;");

    boatSelector_ = new QComboBox(topBar);
    boatSelector_->addItems({"boat1", "boat2", "boat3"});
    boatSelector_->setStyleSheet(R"(
        QComboBox {
            background-color: white;
            color: black;
            border: 1px solid #888;
            padding: 2px 6px;
        }
        QComboBox QAbstractItemView {
            background-color: white;
            selection-background-color: #0078d7;
            color: black;
        }
    )");

    connect(boatSelector_, &QComboBox::currentTextChanged,
            this, &MainWindow::onBoatChanged);

    // ========== 控制状态按钮组 ==========
        // AUTO按钮
        autoBtn_ = new QPushButton("AUTO", topBar);
        autoBtn_->setFixedSize(80, 30);
        autoBtn_->setCheckable(true);

        // MANUAL按钮
        manualBtn_ = new QPushButton("MANUAL", topBar);
        manualBtn_->setFixedSize(80, 30);
        manualBtn_->setCheckable(true);

        // STOP按钮
        stopBtn_ = new QPushButton("STOP", topBar);
        stopBtn_->setFixedSize(80, 30);
        stopBtn_->setCheckable(true);
        stopBtn_->setChecked(true); // 默认选中STOP

        // 设置按钮样式
        QString buttonStyle = R"(
            QPushButton {
                background-color: #f0f0f0;
                color: #333;
                font-size: 12px;
                font-weight: bold;
                border: 2px solid #888;
                border-radius: 4px;
            }
            QPushButton:checked {
                background-color: #0078d7;
                color: white;
                border-color: #005a9e;
            }
            QPushButton:hover:!checked {
                background-color: #e0e0e0;
            }
            QPushButton:pressed {
                background-color: #005a9e;
            }
        )";

        autoBtn_->setStyleSheet(buttonStyle);
        manualBtn_->setStyleSheet(buttonStyle);
        stopBtn_->setStyleSheet(buttonStyle);

        // 连接按钮信号
        connect(autoBtn_, &QPushButton::clicked, this, &MainWindow::onAutoClicked);
        connect(manualBtn_, &QPushButton::clicked, this, &MainWindow::onManualClicked);
        connect(stopBtn_, &QPushButton::clicked, this, &MainWindow::onStopClicked);

    // =========================================
    //  设置波浪参数按钮
    // =========================================
    waveConfigBtn_ = new QPushButton("🌊 设置", topBar);
    waveConfigBtn_->setFixedHeight(32);
    waveConfigBtn_->setStyleSheet(R"(
        QPushButton {
            background-color: white;
            color: black;
            font-size: 14px;
            border: 1px solid #888;
            border-radius: 4px;
            padding: 4px 12px;
        }
        QPushButton:hover {
            background-color: #0078d7;
            color: white;
        }
        QPushButton:disabled {
            background-color: #f0f0f0;
            color: #888;
        }
    )");

    connect(waveConfigBtn_, &QPushButton::clicked,
            this, &MainWindow::onOpenWaveConfig);

    // 布局管理
        topLayout->addWidget(boatLabel);
        topLayout->addWidget(boatSelector_);
        topLayout->addStretch();
        topLayout->addWidget(autoBtn_);
        topLayout->addWidget(manualBtn_);
        topLayout->addWidget(stopBtn_);
        topLayout->addStretch();
        topLayout->addWidget(waveConfigBtn_);
        topBar->setLayout(topLayout);

    // 顶部栏背景设为白色
    topBar->setAutoFillBackground(true);
    topBar->setStyleSheet("background-color: white;");

    // ========== 仪表盘部分 ==========
    dashboard_ = new DashBoard(this);

    // ========== 主布局 ==========
    QWidget* central = new QWidget(this);
    auto* vLayout = new QVBoxLayout(central);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->setSpacing(0);
    vLayout->addWidget(topBar);
    vLayout->addWidget(dashboard_, 1);
    central->setLayout(vLayout);

    // 整体背景设为黑色（仪表盘部分）
    central->setStyleSheet("background-color: black;");
    setCentralWidget(central);

    // ========== 状态栏 ==========
    statusBar()->setStyleSheet("background-color: #888888");
    statusBar()->showMessage("系统已启动，当前控制船只: boat1，控制状态: STOP");

    currentBoat_ = "boat1";
    emit sendBoatSelectionToMqtt(currentBoat_);
    emit sendControlStatusToMqtt(currentControlStatus_); // 发送初始状态
    // 初始化默认波浪配置
        currentWaveConfig_.amplitude = 1.0;
        currentWaveConfig_.period = 5.0;
        currentWaveConfig_.direction = 0.0;
        currentWaveConfig_.steepness = 0.1;
        currentWaveConfig_.gain = 1.0;
}

// ========== 控制按钮槽函数实现 ==========

void MainWindow::onAutoClicked()
{
    if (currentControlStatus_ == "AUTO") return;

    // 设置按钮状态
    autoBtn_->setChecked(true);
    manualBtn_->setChecked(false);
    stopBtn_->setChecked(false);

    currentControlStatus_ = "AUTO";

    QString msg = tr("控制模式切换为: AUTO");
    statusBar()->showMessage(msg);
    qDebug() << "[CONTROL]" << msg;

    // 发送控制状态到MQTT
    emit sendControlStatusToMqtt(currentControlStatus_);
}

void MainWindow::onManualClicked()
{
    if (currentControlStatus_ == "MANUAL") return;

    // 设置按钮状态
    autoBtn_->setChecked(false);
    manualBtn_->setChecked(true);
    stopBtn_->setChecked(false);

    currentControlStatus_ = "MANUAL";

    QString msg = tr("控制模式切换为: MANUAL");
    statusBar()->showMessage(msg);
    qDebug() << "[CONTROL]" << msg;

    // 发送控制状态到MQTT
    emit sendControlStatusToMqtt(currentControlStatus_);
}

void MainWindow::onStopClicked()
{
    if (currentControlStatus_ == "STOP") return;

    // 设置按钮状态
    autoBtn_->setChecked(false);
    manualBtn_->setChecked(false);
    stopBtn_->setChecked(true);

    currentControlStatus_ = "STOP";

    QString msg = tr("控制模式切换为: STOP");
    statusBar()->showMessage(msg);
    qDebug() << "[CONTROL]" << msg;

    // 发送控制状态到MQTT
    emit sendControlStatusToMqtt(currentControlStatus_);
}

/**
 * @brief 当用户切换船只时触发
 */
void MainWindow::onBoatChanged(const QString& name)
{
    currentBoat_ = name;  // 更新当前船只变量
    QString msg = tr("当前选择船只: %1").arg(name);
    statusBar()->showMessage(msg);
    qDebug() << "[INFO]" << msg;

    // TODO: 将当前选中船只发送给服务器
    emit sendBoatSelectionToMqtt(name);
}

/**
 * @brief 打开波浪参数配置对话框
 */
void MainWindow::onOpenWaveConfig()
{
    WaveConfigDialog dlg(this);

    //  在打开对话框前，设置当前配置
    dlg.setConfig(currentWaveConfig_);

    //  接收新配置
    connect(&dlg, &WaveConfigDialog::configChanged,
            this, &MainWindow::onWaveConfigChanged);

    dlg.exec();  // 模态方式显示
}

/**
 * @brief 接收波浪参数变化信号
 */
void MainWindow::onWaveConfigChanged(const WaveConfig& cfg)
{
    QString msg = tr("风浪配置更新 → 幅值:%1m 周期:%2s 方向:%3rad 陡峭:%4 增益:%5")
                      .arg(cfg.amplitude)
                      .arg(cfg.period)
                      .arg(cfg.direction)
                      .arg(cfg.steepness)
                      .arg(cfg.gain);
    statusBar()->showMessage(msg);
    qDebug() << "[WAVE CONFIG]" << msg;

    currentWaveConfig_ = cfg;  // 保存当前配置
    // TODO: 后续这里可以把 cfg 与 currentBoat_ 一起发送给服务器
    emit sendWaveConfigToMqtt(cfg);
}

/* ============================================================
 * Dashboard 更新接口 —— 解耦 Dashboard 与外部模块
 * ============================================================*/

/**
 * @brief 更新风仪表
 */
void MainWindow::updateWind(double direction, double speed, double roll)
{
    if (dashboard_) dashboard_->setWind(direction, speed, roll);
}

/**
 * @brief 更新导航信息（经纬度、高度、航向、航速、俯仰）
 */
void MainWindow::updateNav(double lat, double lon, double alt,
                           double cog, double sog, double pitch)
{
    if (dashboard_) dashboard_->setNav(lat, lon, alt, cog, sog, pitch);
}

/**
 * @brief 更新引擎推力与舵角
 */
void MainWindow::updateEngines(double portOrder, double portRudder,
                               double stbdOrder, double stbdRudder)
{
    if (dashboard_) dashboard_->setEngines(portOrder, portRudder, stbdOrder, stbdRudder);
}

/**
 * @brief 更新推进系统（艏推、主速、艉推）
 */
void MainWindow::updatePropulsion(double bowThrust, double mainSpeed, double sternThrust)
{
    if (dashboard_) dashboard_->setPropulsion(bowThrust, mainSpeed, sternThrust);
}

/**
 * @brief 更新加速度与舵角显示（三角标记）
 */
void MainWindow::updateSpeedAndRudder(double accel, double mainSpeed,
                                      double rudder_port, double rudder_stbd)
{
    if (dashboard_) dashboard_->setSpeedAndRudder(accel, mainSpeed, rudder_port, rudder_stbd);
}

// 添加波浪配置发布完成后的处理
void MainWindow::onWaveConfigPublished(const WaveConfig& config)
{
    QString msg = tr("波浪配置已发送到船只: %1").arg(currentBoat_);
    statusBar()->showMessage(msg);
    qDebug() << "[MQTT] 波浪配置发布成功:" << msg;
}
