#include "engine/port_engine_container.h"
#include <QFrame>
#include <QVBoxLayout>
#include <QLabel>

/**
 * @brief PortEngineContainer —— 左舷引擎容器（仅保留能量条 + 舵角仪）
 */
PortEngineContainer::PortEngineContainer(QWidget* parent)
    : QWidget(parent)
{
    // ===== 背景与整体布局 =====
    // setAutoFillBackground(true);
    // setStyleSheet("background-color: black; color: white;");
    this->setStyleSheet("background-color: black;");  // 控制整体背景


    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(12, 12, 12, 12);

    // ===== 顶部红条 PORT =====
    QFrame* titleBar = new QFrame(this);
    titleBar->setFixedHeight(24);
    titleBar->setStyleSheet("background-color: rgb(180,40,40); border: none;");
    auto* titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    QLabel* title = new QLabel("PORT", titleBar);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color: white; font-weight: bold; font-size: 16pt;");
    titleLayout->addWidget(title);
    mainLayout->addWidget(titleBar);

    // ===== 中间能量条部分 =====
    QWidget* centerWidget = new QWidget(this);
    auto* centerLayout = new QVBoxLayout(centerWidget);
    centerLayout->setContentsMargins(0, 20, 0, 20);  // ✅ 减少留白，释放垂直空间
    centerLayout->setSpacing(5);

    QLabel* engLabel = new QLabel("ENG", this);
    QFont f = engLabel->font();
    f.setPointSize(20);
    f.setBold(true);
    engLabel->setFont(f);
    engLabel->setAlignment(Qt::AlignHCenter);
    engLabel->setStyleSheet("color: #9E9E9E; font-weight: bold;");

    //  能量条：设置固定最小高度，防止被压缩
    bar_ = new EngineBarWidget(this);
    bar_->setFixedWidth(200);
    bar_->setMinimumHeight(500);   // 给足最小绘制高度（保证所有刻度可见）
    bar_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred); // 允许扩展但不强制压缩

    // 中间布局
    centerLayout->addWidget(engLabel, 0, Qt::AlignLeft);
    centerLayout->addStretch(1);
    centerLayout->addWidget(bar_, 0, Qt::AlignHCenter);
    centerLayout->addStretch(1);

    // 伸缩比例设为 1，避免布局挤压
    mainLayout->addWidget(centerWidget, 1);

    // ===== 底部 RUDDER =====
    rudder_ = new RudderGaugeWidget(this);
    rudder_->setMinimumHeight(180);  // 稍微小一点释放空间
    rudder_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    mainLayout->addWidget(rudder_, 0, Qt::AlignBottom);
}

/**
 * @brief 更新数据
 */
void PortEngineContainer::setData(double order, double rudder)
{
    bar_->setValue(order);
    rudder_->setRudderAngle(rudder);
}
