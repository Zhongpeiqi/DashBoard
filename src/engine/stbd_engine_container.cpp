#include "engine/stbd_engine_container.h"
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

/**
 * @brief StbdEngineContainer —— 右舷引擎容器
 * 实现说明：
 *  1. 顶部绿色横条显示“STBD”；
 *  2. 中部垂直能量条 + 标签 ENG；
 *  3. 底部舵角仪 RudderGaugeWidget；
 *  4. 整体布局与左舷容器一致，只是颜色与标题不同。
 */
StbdEngineContainer::StbdEngineContainer(QWidget* parent)
    : QWidget(parent)
{
    // ===== 背景与主布局 =====
    // setAutoFillBackground(true);
    // setStyleSheet("background-color: black; color: white;");
    this->setStyleSheet("background-color: black;");  // 控制整体背景


    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(12, 12, 12, 12);

    // ===== 顶部绿色标题条 “STBD” =====
    QFrame* titleBar = new QFrame(this);
    titleBar->setFixedHeight(24);
    //  改为绿色条
    titleBar->setStyleSheet("background-color: rgb(40,180,40); border: none;");
    auto* titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(0, 0, 0, 0);

    QLabel* title = new QLabel("STBD", titleBar);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color: white; font-weight: bold; font-size: 16pt;");
    titleLayout->addWidget(title);
    mainLayout->addWidget(titleBar);

    // ===== 中部：引擎能量条部分 =====
    QWidget* centerWidget = new QWidget(this);
    auto* centerLayout = new QVBoxLayout(centerWidget);
    centerLayout->setContentsMargins(0, 20, 0, 20);
    centerLayout->setSpacing(5);

    QLabel* engLabel = new QLabel("ENG", this);
    QFont f = engLabel->font();
    f.setPointSize(20);
    f.setBold(true);
    engLabel->setFont(f);
    engLabel->setAlignment(Qt::AlignHCenter);
    engLabel->setStyleSheet("color: #9E9E9E; font-weight: bold;");

    //  能量条
    bar_ = new EngineBarWidget(this);
    bar_->setFixedWidth(200);
    bar_->setMinimumHeight(500);
    bar_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    // 垂直居中布局
    centerLayout->addWidget(engLabel, 0, Qt::AlignLeft);
    centerLayout->addStretch(1);
    centerLayout->addWidget(bar_, 0, Qt::AlignHCenter);
    centerLayout->addStretch(1);

    mainLayout->addWidget(centerWidget, 1);

    // ===== 底部：舵角仪表 =====
    rudder_ = new RudderGaugeWidget(this);
    rudder_->setMinimumHeight(180);
    rudder_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    mainLayout->addWidget(rudder_, 0, Qt::AlignBottom);
}

/**
 * @brief setData 更新右舷引擎数据
 * @param order 引擎功率命令（例如 0~100%）
 * @param rudder 当前舵角（单位：度）
 */
void StbdEngineContainer::setData(double order, double rudder)
{
    bar_->setValue(order);
    rudder_->setRudderAngle(rudder);
}
