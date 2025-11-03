#pragma once
#include <QWidget>

/**
 * WindRoseWidget —— 风向盘控件
 */
class WindRoseWidget : public QWidget {
    Q_OBJECT
public:
    explicit WindRoseWidget(QWidget* parent = nullptr);
    ~WindRoseWidget() override = default;

    void setTrueDirectionDeg(double deg)      { true_dir_deg_ = deg; update(); }
    void setTrueSpeed(double kn)              { true_spd_kn_  = kn; update(); }

protected:
    void paintEvent(QPaintEvent*) override;

private:
    double true_dir_deg_{270.0};   // 真风向
    double true_spd_kn_{13.0};     // 真风速

    QColor trueColor_{Qt::white};
    QColor relColor_{QColor(86, 156, 214)};

    // 工具函数：角度→圆上的点
    static QPointF polarPoint(const QPointF& c, double r, double deg_from_top_cw);

    // 绘制刻度与数字
    void drawScale(QPainter& p, const QRectF& rc, double r, double& scale_outer_radius);
    // 绘制倒三角指针
    void drawPointerTriangle(QPainter& p, const QPointF& c, double r, double deg_from_top_cw, const QColor& color);

};
