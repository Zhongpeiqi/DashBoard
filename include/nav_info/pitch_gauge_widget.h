#pragma once

#include <QWidget>
#include <QPropertyAnimation>

/**
 * @brief PitchGaugeWidget —— 俯仰角仪表控件
 *
 * 功能说明：
 *  - 左上角标题“PITCH”；
 *  - 右上角显示当前俯仰角值（实时更新，动画过程中同步变化）；
 *  - 中部绘制右侧半圆弧刻度（范围 -15° ~ +15°）；
 *  - 显示刻度 -15/-10/-5/0/5/10/15（仅 0 和 ±10 标数字）；
 *  - 白色三角形指针随 pitch_ 动态转动；
 *  - 平滑动画显示，使用 QPropertyAnimation 实现；
 *  - 可自适应大小缩放；
 *  - 自动抗锯齿绘制。
 */
class PitchGaugeWidget : public QWidget {
    Q_OBJECT
    // 定义可动画属性 pitchValue，对应 getPitchValue/setPitchValue
    Q_PROPERTY(double pitchValue READ getPitchValue WRITE setPitchValue)

public:
    explicit PitchGaugeWidget(QWidget* parent = nullptr);

    /**
     * @brief 设置俯仰角值
     * @param val 范围 [-15°, +15°]
     * 自动使用动画平滑过渡，并实时显示中间值。
     */
    void setPitch(double val);

    // 获取当前俯仰角值（供动画读取）
    double getPitchValue() const { return pitch_; }

    // 动画更新函数（每帧调用）
    void setPitchValue(double val);

protected:
    void paintEvent(QPaintEvent*) override;

private:
    double pitch_ = 0.0;                 ///< 当前俯仰角值（单位°）
    QPropertyAnimation* anim_ = nullptr; ///< 动画控制器

    // 工具函数：物理角 (-15~15) 映射到弧角 (-60~60)
    static inline double arcDegFromPhys(double physDeg) {
        return physDeg * 4.0;
    }
};
