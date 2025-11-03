#pragma once
#include <QWidget>
#include <QTimer>

/**
 * @brief RudderGaugeWidget —— 平滑舵角仪表
 *
 *  - 舵角指针平滑过渡显示
 *  - 类似真实船舶舵机带惯性/阻尼效果
 */
class RudderGaugeWidget : public QWidget {
    Q_OBJECT
public:
    explicit RudderGaugeWidget(QWidget* parent = nullptr);

    // 设置舵角角度（范围：-40 ~ 40°）
    void setRudderAngle(double val);

protected:
    void paintEvent(QPaintEvent*) override;

private:
    double targetAngle_ = 0.0;     // 外部传入的舵角目标值
    double displayAngle_ = 0.0;    // 平滑显示值（内部缓动）
    QTimer* smoothTimer_;          // 平滑定时器
};
