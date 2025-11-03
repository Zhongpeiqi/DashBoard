#include "engine/engine_bar_widget.h"
#include <QPainter>
#include <QPen>
#include <QFont>
#include <QFontMetrics>
#include <QtMath>
#include <QDebug>

/**
 * @brief 构造函数
 * 增加一个定时器用于渐变推进显示
 */
EngineBarWidget::EngineBarWidget(QWidget* parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // --- 平滑动画定时器 ---
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {

        // 平滑系数（越小越平滑，越大越灵敏）
        const double alpha = 0.10;

        // 核心缓动算法：逐步逼近目标值
        displayValue_ += (value_ - displayValue_) * alpha;

        // // 输出调试信息
        // qDebug() << "[EngineBarWidget] Target:" << value_
        //          << " Display:" << displayValue_;

        update(); // 刷新界面
    });
    timer->start(16); // ~60 FPS 刷新
}

/**
 * @brief 设置当前目标值（-100 ~ +100）
 * 不直接更新显示，而是让显示慢慢靠近
 */
void EngineBarWidget::setValue(double v)
{
    v = qBound(-100.0, v, 100.0);
    value_ = v;

    // qDebug() << "[EngineBarWidget] setValue received:" << v;
}

/**
 * @brief 绘制刻度线 + 数字 + 双向能量条（使用 displayValue_）
 */
void EngineBarWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), QColor(0, 0, 0));

    // ---------- 绘图边界 ----------
    const double marginLeft   = 10.0;
    const double marginRight  = 10.0;
    const double marginTop    = 20.0;
    const double marginBottom = 20.0;
    const double drawLeft     = marginLeft;
    const double drawRight    = width() - marginRight;

    // ---------- 布局参数 ----------
    const double labelWidth   = 56.0;
    const double labelPadding = 18.0;
    const double barW         = 50.0;

    const double scaleX1 = drawLeft + labelWidth;
    const double scaleX2 = scaleX1 + 200.0;  // 刻度线长度
    double barX = drawRight - barW - 33;

    const int minV = -100, maxV = 100, step = 20;
    const int tickCount = (maxV - minV) / step + 1;
    const double tickSpacing = 36.0;
    const double totalHeight = (tickCount - 1) * tickSpacing;
    const double centerY = height() / 2.0;
    const double drawTop = centerY - totalHeight / 2.0;
    const double drawBottom = centerY + totalHeight / 2.0;

    // ---------- 字体 ----------
    QFont tickFont("Arial", 11, QFont::Bold);
    p.setFont(tickFont);
    QFontMetrics fm(tickFont);
    QPen tickPen(QColor(255, 255, 255), 2.0);
    p.setPen(tickPen);

    // ---------- 1. 刻度线 + 数字 ----------
    for (int i = 0; i < tickCount; ++i)
    {
        int val = maxV - i * step;
        double y = drawTop + i * tickSpacing;
        p.drawLine(QPointF(scaleX1, y), QPointF(scaleX2, y));
        QString txt = QString::number(abs(val));
        int txtW = fm.horizontalAdvance(txt);
        int tx = scaleX1 - labelPadding - txtW;
        int ty = y + fm.ascent() / 3;
        p.drawText(tx, ty, txt);
    }

    // ---------- 2. 绘制 0 线 ----------
    const double zeroY = drawTop + 5 * tickSpacing;
    p.setPen(QPen(QColor(255, 255, 255), 2.8));
    p.drawLine(QPointF(scaleX1, zeroY), QPointF(scaleX2, zeroY));

    // ---------- 3. 绘制平滑能量条 ----------
    double ratio = std::abs(displayValue_) / 100.0;

    double yTop, yBottom;
    if (displayValue_ >= 0) {
        yTop = zeroY - ratio * (zeroY - drawTop);
        yBottom = zeroY;
    } else {
        yTop = zeroY;
        yBottom = zeroY + ratio * (drawBottom - zeroY);
    }

    QRectF barRect(barX, yTop, barW, yBottom - yTop);
    if (barRect.top() < drawTop) barRect.setTop(drawTop);
    if (barRect.bottom() > drawBottom) barRect.setBottom(drawBottom);

    QLinearGradient grad(barRect.topLeft(), barRect.bottomLeft());
    grad.setColorAt(0.0, QColor(0, 255, 0, 255));
    grad.setColorAt(1.0, QColor(0, 150, 0, 230));

    p.setPen(Qt::NoPen);
    p.fillRect(barRect, grad);

    // 高光线
    p.setPen(QPen(QColor(255, 255, 255, 180), 1.0));
    p.drawLine(QPointF(barRect.left(), yTop), QPointF(barRect.right(), yTop));
}
