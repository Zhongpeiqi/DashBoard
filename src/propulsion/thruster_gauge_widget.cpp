#include "propulsion/thruster_gauge_widget.h"
#include <QPainter>
#include <QFont>
#include <QtMath>

ThrusterGaugeWidget::ThrusterGaugeWidget(QWidget* parent)
    : QWidget(parent)
{
    // setMinimumSize(200, 200);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void ThrusterGaugeWidget::setThrust(double thrust)
{
    if (thrust < -10) thrust = -10;
    if (thrust > 10)  thrust = 10;
    thrust_ = thrust;
    update();
}
void ThrusterGaugeWidget::setLabel(const QString& text)
{
    label_ = text;
    update();  // 文字变化后自动重绘
}

void ThrusterGaugeWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.fillRect(rect(), Qt::black);  //  背景纯黑

    const int w = width();
    const int h = height();
    const int margin = 26;
    const int centerY = h / 2 + 10;
    const int zeroX = w / 2;

    //  使用完全不透明的纯白
    QColor tickColor = Qt::white;
    QColor textColor = QColor("#9E9E9E");


    // ================= 绘制标题（左上角） =================
    p.setPen(textColor);
    QFont titleFont("Arial", 16, QFont::Bold);
    p.setFont(titleFont);
    p.drawText(10, 24, "THRUSTER "+label_);

    // ================= 绘制刻度 =================
    QFont font("Arial", 10, QFont::Bold);
    p.setFont(font);

    //  禁用抗锯齿，使线条像素完全白
    p.setRenderHint(QPainter::Antialiasing, false);
    QPen pen(tickColor);
    pen.setWidth(2);  //  加粗线条避免灰化
    pen.setCapStyle(Qt::FlatCap);
    p.setPen(pen);

    double tickSpacing = (w - 2 * margin) / 20.0;
    const int baseY = centerY;

    for (int i = -10; i <= 10; ++i)
    {
        int x = zeroX + static_cast<int>(i * tickSpacing);
        int tickHeight = (i % 2 == 0) ? 10 : 6;

        //  绘制刻度线（完全不透明纯白）
        p.drawLine(x, baseY - tickHeight, x, baseY + tickHeight);
    }

    //  启用抗锯齿（仅用于文字，保证字体平滑）
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(textColor);

    // 绘制刻度数字（仅偶数刻度）
    for (int i = -10; i <= 10; i += 2)
    {
        int x = zeroX + static_cast<int>(i * tickSpacing);
        QString label = QString::number(qAbs(i));
        int textWidth = p.fontMetrics().horizontalAdvance(label);
        p.drawText(x - textWidth / 2, baseY + 28, label);
    }
}
