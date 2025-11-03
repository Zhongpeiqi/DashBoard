#pragma once
#include <QWidget>
#include <QPropertyAnimation>

/**
 * @brief RollGaugeWidget —— 横滚角指示器
 *
 * 显示范围：-25° ~ +25°
 *  - 中间为 0°，左右对称；
 *  - 每隔 5° 有细刻度，每隔 10° 有长刻度与数字；
 *  - 绿色倒三角指针表示当前横滚角；
 *  - 动画平滑滑动；
 *  - 右上角显示实时横滚角值；
 */
class RollGaugeWidget : public QWidget {
    Q_OBJECT
    // 定义可动画属性 rollValue
    Q_PROPERTY(double rollValue READ getRollValue WRITE setRollValue)

public:
    explicit RollGaugeWidget(QWidget* parent = nullptr);
    ~RollGaugeWidget() override = default;

    /**
     * @brief 外部接口：设置横滚角度（单位：度）
     * 平滑动画更新。
     */
    void setRollDeg(double deg);

    double getRollValue() const { return roll_deg_; }
    void setRollValue(double val); // 动画中每帧调用

protected:
    void paintEvent(QPaintEvent*) override;
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

private:
    double roll_deg_{0.0};              ///< 当前横滚角
    QPropertyAnimation* anim_{nullptr}; ///< 动画控制器

    // 辅助函数：根据圆心坐标与角度计算圆周点
    static QPointF pointOnCirclePhi(const QPointF& c, double r, double phi_deg);
    // 将 roll 值(-25°~+25°)映射到弧线角(200°~340°)
    static double valueToArcPhi(double v);
};
