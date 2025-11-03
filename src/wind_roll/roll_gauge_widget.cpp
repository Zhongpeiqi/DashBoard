#include "wind_roll/roll_gauge_widget.h"
#include <QPainter>
#include <QtMath>
#include <QPainterPath>
#include <QSvgRenderer>
#include <QEasingCurve>

namespace {
constexpr double kArcStartPhi = 200.0;
constexpr double kArcEndPhi   = 340.0;
}

// ============================================================
// 构造函数
// ============================================================
RollGaugeWidget::RollGaugeWidget(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setMinimumHeight(150);

    // 初始化动画控制器
    anim_ = new QPropertyAnimation(this, "rollValue");
    anim_->setDuration(500);                      // 平滑滑动时间
    anim_->setEasingCurve(QEasingCurve::InOutQuad); // 缓入缓出曲线
}

// ============================================================
// 动画属性：每帧更新 roll 值并重绘
// ============================================================
void RollGaugeWidget::setRollValue(double val)
{
    roll_deg_ = val;
    update();
}

// ============================================================
// 外部接口：设置目标 roll 值（带动画）
// ============================================================
void RollGaugeWidget::setRollDeg(double deg)
{
    if (deg < -25.0) deg = -25.0;
    if (deg >  25.0) deg =  25.0;

    if (anim_->state() == QAbstractAnimation::Running)
        anim_->stop();

    anim_->setStartValue(roll_deg_);
    anim_->setEndValue(deg);
    anim_->start();
}

// ============================================================
// 计算圆上指定角度的点
// ============================================================
QPointF RollGaugeWidget::pointOnCirclePhi(const QPointF& c, double r, double phi_deg)
{
    const double rad = qDegreesToRadians(phi_deg);
    return { c.x() + r * std::cos(rad), c.y() - r * std::sin(rad) };
}

// ============================================================
// 将 roll 值(-25°~+25°)映射到弧线角(200°~340°)
// ============================================================
double RollGaugeWidget::valueToArcPhi(double v)
{
    if (v < -25.0) v = -25.0;
    if (v >  25.0) v =  25.0;
    double t = (v + 25.0) / 50.0; // -25→0, +25→1
    return kArcStartPhi + (kArcEndPhi - kArcStartPhi) * t;
}

// ============================================================
// 绘制事件：核心绘图逻辑
// ============================================================
void RollGaugeWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.fillRect(rect(), QColor(0, 0, 0));  // 背景黑色
    p.setRenderHint(QPainter::Antialiasing, true);

    QRectF rc = rect().adjusted(20, 10, -20, -10);
    double r = rc.width() * 0.53;
    QPointF C(rc.center().x(), rc.bottom() - r * 0.9 - 40);

    // -------------------- 1. 绘制标题与实时数值 --------------------
    QFont f("Microsoft YaHei");
    f.setBold(true);

    // 左上角标题 —— 固定相对容器边距绘制
    f.setPointSize(20);
    p.setFont(f);
    p.setPen(QColor("#9E9E9E"));
    QFontMetrics fmTitle(f);
    int marginX = 20;
    int marginY = fmTitle.ascent() + 10;  // 顶部留白
    p.drawText(marginX, marginY, "ROLL");

    // 右上角实时数值 —— 自动右对齐
    f.setPointSize(25);
    p.setFont(f);
    p.setPen(Qt::white);
    QString valText = QString::number(fabs(roll_deg_), 'f', 2) + QString::fromUtf8("°");
    QFontMetrics fmVal(f);
    int valX = width() - fmVal.horizontalAdvance(valText) - marginX;
    int valY = fmVal.ascent() + 10;
    p.drawText(valX, valY, valText);


    // -------------------- 2. 绘制主弧 --------------------
    p.setPen(QPen(QColor(200, 210, 220), 2));
    QPointF prev = pointOnCirclePhi(C, r, kArcStartPhi);
    for (double phi = kArcStartPhi + 1.0; phi <= kArcEndPhi; phi += 1.0) {
        QPointF cur = pointOnCirclePhi(C, r, phi);
        p.drawLine(prev, cur);
        prev = cur;
    }

    // -------------------- 3. 绘制刻度与数字 --------------------
    const double majorLen = r * 0.08;
    const double minorLen = r * 0.04;
    const double labelOff = 24.0;

    for (int v = -25; v <= 25; v += 5) {
        double phi = valueToArcPhi(v);
        QPointF A = pointOnCirclePhi(C, r, phi);
        QPointF B = pointOnCirclePhi(C, r - ((v % 10 == 0) ? majorLen : minorLen), phi);

        p.setPen(QPen(QColor(220, 220, 220), (v % 10 == 0) ? 2 : 1));
        p.drawLine(A, B);

        if (v == 0 || v == 10 || v == -10 || v == 20 || v == -20) {
            QString txt = QString::number(std::abs(v));
            QPointF T = pointOnCirclePhi(C, r + labelOff, phi);
            p.setPen(QColor(230, 230, 230));
            f.setPointSize(16);
            f.setBold(true);
            p.setFont(f);
            p.drawText(QRectF(T.x() - 18, T.y() - 12, 36, 24), Qt::AlignCenter, txt);
        }
    }

    // -------------------- 4. 绘制绿色倒三角指针 --------------------
    double phi_val = valueToArcPhi(roll_deg_);
    double tip_radius = r - majorLen * 1.4;
    QPointF tip = pointOnCirclePhi(C, tip_radius, phi_val);
    double rad = qDegreesToRadians(phi_val);

    QPointF tangent(-std::sin(rad), -std::cos(rad));
    QPointF inward(-std::cos(rad),  std::sin(rad));

    double base_half = r * 0.08;
    double height    = r * 0.12;
    QPointF baseC = tip + inward * height;
    QPointF L = baseC - tangent * base_half;
    QPointF R = baseC + tangent * base_half;

    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0, 255, 0));
    QPolygonF tri; tri << tip << L << R;
    p.drawPolygon(tri);

    // -------------------- 5. 绘制 SVG 船图标 --------------------
    QSvgRenderer svgRenderer(QStringLiteral(":/icons/images/ship_roll.svg"));
    double shipWidth = r * 0.5;
    double shipHeight = r * 0.4;
    QRectF shipRect(C.x() - shipWidth / 2.0, C.y(), shipWidth, shipHeight);
    svgRenderer.render(&p, shipRect);

    // -------------------- 6. 绘制水线 --------------------
    QColor shipColor(158, 158, 158);
    double waterlineY = shipRect.top() + shipRect.height() * 0.75;
    double waterlineLen = shipRect.width() * 1.5;
    double waterlineX1 = shipRect.center().x() - waterlineLen / 2.0;
    double waterlineX2 = shipRect.center().x() + waterlineLen / 2.0;

    double shipVisibleHalf = shipRect.width() * 0.34;
    double shipVisibleLeft = shipRect.center().x() - shipVisibleHalf;
    double shipVisibleRight = shipRect.center().x() + shipVisibleHalf;

    p.setPen(QPen(shipColor, 2));
    p.drawLine(QPointF(waterlineX1, waterlineY), QPointF(shipVisibleLeft, waterlineY));
    p.drawLine(QPointF(shipVisibleRight, waterlineY), QPointF(waterlineX2, waterlineY));
}

// ============================================================
// 推荐与最小尺寸
// ============================================================
QSize RollGaugeWidget::sizeHint() const { return QSize(400, 160); }
QSize RollGaugeWidget::minimumSizeHint() const { return QSize(220, 100); }
