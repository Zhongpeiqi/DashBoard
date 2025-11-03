#include "engine/rudder_gauge_widget.h"
#include <QPainter>
#include <QtMath>
#include <QDebug>
/**
 * @brief 构造函数：初始化平滑计时器
 */
RudderGaugeWidget::RudderGaugeWidget(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setMinimumHeight(120);

    // ===== 平滑动画定时器 =====
    smoothTimer_ = new QTimer(this);
    connect(smoothTimer_, &QTimer::timeout, this, [this]() {

        // 平滑系数：越小越柔和（真实舵机惯性）
        const double alpha = 0.10;   // 10% 逼近目标，可调 0.05~0.2

        // 更新显示值（缓动）
        displayAngle_ += (targetAngle_ - displayAngle_) * alpha;

        // // Debug 输出
        // qDebug() << "[Rudder] Target:" << targetAngle_
        //          << " Display:" << displayAngle_;

        update();
    });

    smoothTimer_->start(16); // ~100 FPS
}

/**
 * @brief 设置舵角（输入命令）
 */
void RudderGaugeWidget::setRudderAngle(double val)
{
    val = qBound(-90.0, val, 90.0);  // 限幅
    targetAngle_ = val;

    // qDebug() << "[Rudder] Command angle:" << val;
}

/**
 * @brief 绘制舵角仪表
 */
void RudderGaugeWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), Qt::black);

    const int w = width();
    const int h = height();

    // ========== 标题 ==========
    QFont titleFont("Arial", 18, QFont::Bold);
    p.setFont(titleFont);
    p.setPen(QColor("#9E9E9E"));
    p.drawText(10, 30, "RUDDER");

    // ========== 显示角度值 ==========
    QFont valueFont("Arial", 38, QFont::Bold);
    p.setFont(valueFont);
    p.setPen(Qt::white);

    // 角度显示值，可以显示 -90° 到 90°
    QString angleText = QString::number(fabs(displayAngle_), 'f', 0) + "°";
    p.drawText(QRect(0, 35, w, 60), Qt::AlignCenter, angleText);

    // ========== 画刻度线 ==========
    const double zeroY = h - 35;
    const double spacing = 26.0;
    const int maxAng = 40;  // 刻度最大范围，保持在 ±40°

    QPen tickPen(Qt::white, 2);
    p.setPen(tickPen);
    p.setFont(QFont("Arial", 10, QFont::Bold));

    for (int a = -maxAng; a <= maxAng; a += 10)
    {
        double x = w / 2 + (a / 10.0) * spacing;
        double len = (a % 20 == 0) ? 14 : 8;
        p.drawLine(QPointF(x, zeroY), QPointF(x, zeroY - len));

        if (a % 20 == 0)
        {
            QString txt = QString::number(abs(a));
            p.drawText(QRect(x - 12, zeroY + 5, 24, 14), Qt::AlignCenter, txt);
        }
    }

    // ========== 绿色舵角条 ==========
    // 绿色舵角条最大范围限制为 ±40°，即使实时角度超过这个范围
    double limitedAngle = qBound(-40.0, displayAngle_, 40.0);  // 限制范围在 -40 到 40° 之间
    double ratio = limitedAngle / 40.0;  // 比例计算范围为 ±40°
    double barX = w / 2 + ratio * (4 * spacing);  // 推算绿色舵角条的宽度
    double barW = 18, barH = 10;

    QRectF barRect(std::min(barX, w / 2.0), zeroY - 12,
                   fabs(barX - w / 2), barH);

    QLinearGradient grad(barRect.topLeft(), barRect.bottomLeft());
    grad.setColorAt(0.0, QColor(0, 255, 0));
    grad.setColorAt(1.0, QColor(0, 160, 0));

    p.setPen(Qt::NoPen);
    p.fillRect(barRect, grad);
}
