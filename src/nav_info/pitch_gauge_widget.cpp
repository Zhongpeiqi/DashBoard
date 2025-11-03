#include "nav_info/pitch_gauge_widget.h"
#include <QPainter>
#include <QtMath>
#include <QFontMetrics>
#include <QPolygonF>
#include <QSvgRenderer>
#include <QEasingCurve>

// ============================================================
// 构造函数
// ============================================================
PitchGaugeWidget::PitchGaugeWidget(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 初始化动画属性
    anim_ = new QPropertyAnimation(this, "pitchValue");
    anim_->setEasingCurve(QEasingCurve::InOutQuad);  // 平滑缓入缓出
    anim_->setDuration(300);                         // 默认动画时长 300ms

    setPitchValue(10.0); // 默认显示一个非零角度便于观察
}

// ============================================================
// 设置目标俯仰角（带平滑动画）
// ============================================================
void PitchGaugeWidget::setPitch(double val)
{
    // 限幅防止超出 ±15°
    if (val < -15.0) val = -15.0;
    if (val >  15.0) val =  15.0;

    // 若当前动画正在运行则停止
    if (anim_->state() == QAbstractAnimation::Running)
        anim_->stop();

    // 动画从当前 pitch_ 平滑过渡到目标值
    anim_->setStartValue(pitch_);
    anim_->setEndValue(val);

    // 根据角度差动态调整动画时长（大角度滑动更慢）
    int duration = 120 + static_cast<int>(std::abs(pitch_ - val) * 15);
    anim_->setDuration(duration);

    anim_->start();  // 启动动画
}

// ============================================================
// 动画内部更新函数（每帧调用）
// ============================================================
void PitchGaugeWidget::setPitchValue(double val)
{
    pitch_ = val;  // 更新当前值
    update();      // 刷新绘制（每帧重绘）
}

// ============================================================
// 绘制事件（核心绘制逻辑）
// ============================================================
void PitchGaugeWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.fillRect(rect(), Qt::black); // 背景填充黑色

    const int w = width();
    const int h = height();

    //------------------------- [1] 绘制标题与数值 -------------------------
    QFont f("Microsoft YaHei");
    f.setBold(true);

    // ---- 左上角：灰色标题 ----
    f.setPointSize(20);
    p.setFont(f);
    p.setPen(QColor("#9E9E9E"));
    const int textTop = 45;     // 标题顶部偏移
    const int textLeft = 25;    // 标题左侧偏移
    p.drawText(textLeft - 25, textTop, "PITCH");

    // ---- 右上角：白色数值（实时显示当前动画帧值） ----
    f.setPointSize(25);
    p.setFont(f);
    p.setPen(Qt::white);
    const QString valText = QString::number(fabs(pitch_), 'f', 2) + QString::fromUtf8("°");
    QFontMetrics fm(f);
    const int valX = w - fm.horizontalAdvance(valText);
    const int valY = 55;   // 数值垂直位置
    p.drawText(valX, valY - 5, valText);

    //------------------------- [2] 圆弧参数设置 -------------------------
    // 控制圆心与半径，让弧更大且位置居中
    const QPointF center(w * 0.45, h * 0.60);   // 圆心位置（略靠左）
    const double radius = qMin(w, h) * 0.440;    // 放大半径

    p.setPen(QPen(Qt::white, 2));
    const QRectF arcRect(center.x() - radius, center.y() - radius,
                         radius * 2.0, radius * 2.0);
    p.drawArc(arcRect, (-60) * 16, 120 * 16);   // 绘制右侧 120° 弧

    //------------------------- [2.5] 绘制 SVG 船图标（侧视） -------------------------
    QSvgRenderer shipRenderer(QStringLiteral(":/icons/images/ship_pitch.svg"));

    // 控制 SVG 大小和位置
    double shipWidth = radius * 0.8;
    double shipHeight = radius * 0.50;
    double shipX = center.x() - 85;
    double shipY = center.y() - 45;
    QRectF shipRect(shipX, shipY, shipWidth, shipHeight);

    shipRenderer.render(&p, shipRect);  // 渲染 SVG 船体图标

    // ------------------------- [3] 绘制水平水线（与船体比例关联） -------------------------
    QPen waterlinePen(QColor("#9E9E9E"));
    waterlinePen.setWidth(2);
    p.setPen(waterlinePen);

    // 按比例计算水线位置（约在船底下方 10% 高度处）
    const double lineY = shipRect.bottom() - shipHeight * 0.20;

    // 水线长度与船体宽度成比例
    const double lineMargin = shipWidth * 0.10;  // 水线左右延伸10%
    QPointF pt1(shipRect.left() - lineMargin, lineY);
    QPointF pt2(shipRect.right() + lineMargin, lineY);

    p.drawLine(pt1, pt2);


    //------------------------- [4] 绘制刻度 -------------------------
    QPen tickPen(Qt::white);
    tickPen.setWidth(2);
    p.setPen(tickPen);

    for (int phys = -15; phys <= 15; phys += 5) {
        const double degA = arcDegFromPhys(phys);
        const double radA = qDegreesToRadians(degA);

        const double tickLen = (std::abs(phys) % 10 == 0) ? 22.0 : 14.0;
        const QPointF outer(center.x() + radius * std::cos(radA),
                            center.y() - radius * std::sin(radA));
        const QPointF inner(center.x() + (radius - tickLen) * std::cos(radA),
                            center.y() - (radius - tickLen) * std::sin(radA));
        p.drawLine(outer, inner);

        // ---- 绘制数字 (0, ±10) ----
        if (phys == 0 || std::abs(phys) == 10) {
            const QString label = QString::number(std::abs(phys));
            QFont small("Microsoft YaHei", 14, QFont::Bold);
            p.setFont(small);
            const double textR = radius + 20;
            const QPointF tpos(center.x() + textR * std::cos(radA),
                               center.y() - textR * std::sin(radA));
            const QRectF tb(tpos.x() - 12.0, tpos.y() - 12.0, 24.0, 24.0);
            p.drawText(tb, Qt::AlignCenter, label);
            p.setPen(tickPen);
        }
    }

    //------------------------- [5] 绘制白色三角指针 -------------------------
    const double degPtr = arcDegFromPhys(pitch_);  // 当前俯仰角对应弧角
    const double radPtr = qDegreesToRadians(degPtr);

    const QPointF radial(std::cos(radPtr), -std::sin(radPtr));
    const QPointF tangent(-std::sin(radPtr), -std::cos(radPtr));

    const double tipInset = 20.0;
    const double arrowLength = 18.0;
    const double arrowWidth = 20.0;

    const QPointF tip = center + radial * (radius - tipInset);
    const QPointF baseCenter = tip - radial * arrowLength;
    const QPointF halfSpan = tangent * (arrowWidth * 0.5);
    const QPointF leftPt = baseCenter + halfSpan;
    const QPointF rightPt = baseCenter - halfSpan;

    QPolygonF tri; tri << tip << leftPt << rightPt;
    p.setBrush(Qt::white);
    p.setPen(Qt::NoPen);
    p.drawPolygon(tri);
}
