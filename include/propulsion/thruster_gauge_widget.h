#pragma once
#include <QWidget>

/**
 * @brief ThrusterGaugeWidget —— 推力刻度仪
 * 水平显示从 -10 ~ 10 的推力值刻度。
 */
class ThrusterGaugeWidget : public QWidget {
    Q_OBJECT
public:
    explicit ThrusterGaugeWidget(QWidget* parent = nullptr);

    /// 设置当前推力值（-10 ~ +10）
    void setThrust(double thrust);
    void setLabel(const QString& text);

protected:
    void paintEvent(QPaintEvent*) override;

private:
    double thrust_ = 0.0;  ///< 当前推力值（-10~10）
    QString label_ = "BOW";  // 默认显示 BOW
};
