#include "wind_roll/wind_gauge.h"
#include <QFont>
#include <QDebug>
#include <QPalette>
#include <QSpacerItem>

/**
 * @brief WindGauge 构造函数
 * 综合风向 / 风速 + 横滚角仪表
 */
WindGauge::WindGauge(QWidget *parent)
    : QWidget(parent),
    direction(0.0),
    speed(0.0),
    roll(0.0)
{
    setupLayout();

    // 设置背景纯黑
    setStyleSheet("background-color: rgb(0, 0, 0);");

    // 设置扩展策略
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

/**
 * @brief 初始化布局与控件
 */
void WindGauge::setupLayout()
{
    // ==== 上方 WIND 标签 ====
    titleLabel = new QLabel("WIND", this);
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    titleLabel->setStyleSheet("color: #9E9E9E; font: bold 20px 'Arial';");

    // ==== 中间风玫瑰组件 ====
    windRose = new WindRoseWidget(this);
    windRose->setMinimumSize(200, 200);
    windRose->setAutoFillBackground(false);

    // ==== 下方标签与数值 ====
    QFont labelFont("Arial", 20, QFont::Bold);
    QFont valueFont("Arial", 25, QFont::Bold);

    dirLabel = new QLabel("WDIR", this);
    spdLabel = new QLabel("WSPD", this);

    dirValueLabel = new QLabel("--°", this);
    spdValueLabel = new QLabel("-- m/s", this);

    for (auto lbl : {dirLabel, spdLabel}) {
        lbl->setFont(labelFont);
        lbl->setStyleSheet("color: #9E9E9E;");
    }

    for (auto val : {dirValueLabel, spdValueLabel}) {
        val->setFont(valueFont);
        val->setStyleSheet("color: white;");
    }

    // ==== DIRECTION 一列 ====
    QVBoxLayout *dirCol = new QVBoxLayout();
    dirCol->addWidget(dirLabel, 0, Qt::AlignLeft);
    dirCol->addWidget(dirValueLabel, 0, Qt::AlignLeft);
    dirCol->setSpacing(10);

    // ==== SPEED 一列 ====
    QVBoxLayout *spdCol = new QVBoxLayout();
    spdCol->addWidget(spdLabel, 0, Qt::AlignRight);
    spdCol->addWidget(spdValueLabel, 0, Qt::AlignRight);
    spdCol->setSpacing(10);

    // ==== DIRECTION / SPEED 行 ====
    QHBoxLayout *infoRow = new QHBoxLayout();
    infoRow->addLayout(dirCol);
    infoRow->addStretch();
    infoRow->addLayout(spdCol);
    infoRow->setContentsMargins(15, 0, 15, 0);
    infoRow->setSpacing(10);

    // ==== ROLL 半圆仪表 ====
    rollGauge = new RollGaugeWidget(this);
    rollGauge->setMinimumHeight(150);

    // ==== 主布局 ====
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(titleLabel, 0, Qt::AlignLeft);
    mainLayout->addWidget(windRose, 6);          // 风向盘部分占比
    mainLayout->addLayout(infoRow, 4);           // 信息区（去掉ROLL）
    mainLayout->addWidget(rollGauge, 5, Qt::AlignHCenter); // Roll仪表
    mainLayout->setContentsMargins(5, 5, 5, 5);

    setLayout(mainLayout);
}

/**
 * @brief 设置风向（度）
 */
void WindGauge::setDirection(double dirDeg)
{
    direction = dirDeg;
    updateLabels();
    windRose->setTrueDirectionDeg(direction);
}

/**
 * @brief 设置风速（m/s）
 */
void WindGauge::setSpeed(double spd)
{
    speed = spd;
    updateLabels();
}

/**
 * @brief 设置横滚角（度）
 */
void WindGauge::setRoll(double deg)
{
    roll = fabs(deg);
    rollGauge->setRollDeg(deg);  // 仍然驱动底部仪表
}

/**
 * @brief 刷新标签文字显示
 */
void WindGauge::updateLabels()
{
    dirValueLabel->setText(QString::number(direction, 'f', 1) + QChar(0x00B0));
    spdValueLabel->setText(QString::number(speed, 'f', 1) + " m/s");
}
