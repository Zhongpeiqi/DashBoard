#ifndef WIND_GAUGE_H
#define WIND_GAUGE_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "wind_rose_widget.h"
#include "roll_gauge_widget.h"

/**
 * @brief WindGauge —— 综合风向、风速、横滚角仪表
 *
 * 结构：
 *  ├── 上方标题 QLabel("WIND")
 *  ├── 中间 WindRoseWidget 风向盘
 *  ├── 下方风向 / 风速 数值区
 *  └── 底部 RollGaugeWidget 半圆仪表（仅仪表，无文字标签）
 */
class WindGauge : public QWidget
{
    Q_OBJECT

public:
    explicit WindGauge(QWidget *parent = nullptr);

    /** 设置风向（单位：度） */
    void setDirection(double dirDeg);

    /** 设置风速（单位：m/s） */
    void setSpeed(double spd);

    /** 设置横滚角（单位：度）—— 控制底部仪表 */
    void setRoll(double deg);

private:
    // ===== UI 元素 =====
    QLabel *titleLabel;          // 标题（WIND）

    QLabel *dirLabel;            // “WDIR” 标签
    QLabel *spdLabel;            // “WSPD” 标签

    QLabel *dirValueLabel;       // 风向数值（°）
    QLabel *spdValueLabel;       // 风速数值（m/s）

    WindRoseWidget *windRose;    // 风向图
    RollGaugeWidget *rollGauge;  // 横滚角仪表（半圆）

    // ===== 数据状态 =====
    double direction = 0.0;      // 当前风向
    double speed = 0.0;          // 当前风速
    double roll = 0.0;           // 当前横滚角

    // ===== 辅助函数 =====
    void setupLayout();          // 初始化布局
    void updateLabels();         // 刷新显示数值
};

#endif // WIND_GAUGE_H
