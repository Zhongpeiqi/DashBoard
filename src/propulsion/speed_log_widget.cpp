#include "propulsion/speed_log_widget.h"
#include <QPainter>
#include <QFont>
#include <QtMath>
#include <QPainterPath>
#include <QPolygonF>
#include <QFontMetrics>

SpeedLogWidget::SpeedLogWidget(QWidget* parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void SpeedLogWidget::setSpeed(double accel, double speed, double /*accel_dup*/)
{
    // 中间为当前速度；上下为同一个加速度
    accel_ = accel;
    speed_ = speed;
    update();
}

void SpeedLogWidget::setRudders(double rudder_port, double rudder_stbd)
{
    rudder_port_ = rudder_port;
    rudder_stbd_ = rudder_stbd;
    update();
}

void SpeedLogWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.fillRect(rect(), Qt::black);

    const int w = width();
    const int h = height();
    const QColor kWhite(255, 255, 255);

    // ===================== 标题 =====================
    QFont titleFont("Arial", 18, QFont::Bold);
    p.setFont(titleFont);

    // --- LOG 单独用浅灰色 #9E9E9E ---
    p.setPen(QColor("#9E9E9E"));
    p.drawText(15, 30, "LOG");

    // --- 其他标题恢复白色 ---
    p.setPen(kWhite);
    p.drawText(15, 65, "BT");

    // --- 右侧 Kn ---
    {
        const QString kn = "Kn";
        const int knw = p.fontMetrics().horizontalAdvance(kn);
        p.drawText(w - knw - 15, 65, kn);
    }

    // ===================== 船体轮廓 =====================
    const int centerX      = w / 2;
    const int topY         = 110;    // 顶点固定
    const int bottomMargin = 40;
    const int bottomY      = h - bottomMargin;
    const int shoulderY    = topY + 24;
    const int bodyWidth    = int(w * 0.50);
    const int radius       = 14;

    QPainterPath path;
    path.moveTo(centerX, topY);
    path.quadTo(centerX + bodyWidth * 0.5, topY + 10,
                centerX + bodyWidth * 0.5, shoulderY);
    path.lineTo(centerX + bodyWidth * 0.5, bottomY - radius);
    path.quadTo(centerX + bodyWidth * 0.5, bottomY,
                centerX + bodyWidth * 0.5 - radius, bottomY);
    path.lineTo(centerX - bodyWidth * 0.5 + radius, bottomY);
    path.quadTo(centerX - bodyWidth * 0.5, bottomY,
                centerX - bodyWidth * 0.5, bottomY - radius);
    path.lineTo(centerX - bodyWidth * 0.5, shoulderY);
    path.quadTo(centerX - bodyWidth * 0.5, topY + 10,
                centerX, topY);

    p.setPen(QPen(kWhite, 2));
    p.setBrush(Qt::NoBrush);
    p.drawPath(path);

    // ===================== 三个数值（上=加速度，中=速度，下=加速度） =====================
    QFont numFont("Arial", 22, QFont::Bold);
    p.setFont(numFont);
    p.setPen(kWhite);

    const int midY = (topY + bottomY) / 2;
    const int gap  = 80;               // 三行间距
    const int yTopBase = midY - gap;   // 上：加速度
    const int yMidBase = midY;         // 中：速度
    const int yBotBase = midY + gap;   // 下：加速度

    const QString sAccel = QString::number(fabs(accel_), 'f', 1);
    const QString sSpeed = QString::number(fabs(speed_), 'f', 1);

    QFontMetrics nfm(numFont);
    const int wAccel = nfm.horizontalAdvance(sAccel);
    const int wSpeed = nfm.horizontalAdvance(sSpeed);

    // 居中绘制数值（注意 drawText 的 y 为基线）
    p.drawText(centerX - wAccel / 2, yTopBase, sAccel);
    p.drawText(centerX - wSpeed / 2, yMidBase, sSpeed);
    p.drawText(centerX - wAccel / 2, yBotBase, sAccel);

    // ===================== 三角箭头（白色常显 + 条件绿色高亮） =====================
    drawIndicatorTriangles(p, centerX, yTopBase, yMidBase, yBotBase, bodyWidth);
}

void SpeedLogWidget::drawIndicatorTriangles(QPainter& p,
                                            int cx, int yTopText, int yMidText, int yBotText, int bodyW)
{
    // 颜色
    const QColor kLight(220, 220, 220); // 浅白（常亮）
    const QColor kGreen(0, 255, 0);     // 高亮绿色

    // 上下小三角的尺寸（居中箭头）
    const int halfW = 12;   // 半宽
    const int hTri  = 18;   // 高度
    const int triangleGap = 6; // 与文字间距

    // 外侧左右三角的尺寸（侧翼箭头）
    const int sideA   = 20; // 侧翼箭头的边长
    const int offset  = 25; // 船体外移距离（越大越靠外）
    const int sideXLeft  = cx - int(bodyW * 0.5) - offset;
    const int sideXRight = cx + int(bodyW * 0.5) + offset;

    // —— 工具：画侧翼三角（◀ / ▶），白色底 + 条件绿色盖一层 ——
    auto drawSideArrow = [&](int x, int y, bool right, bool highlight)
    {
        QPolygonF tri;
        if (right) // ▶
            tri << QPointF(x, y - sideA / 2) << QPointF(x + sideA, y) << QPointF(x, y + sideA / 2);
        else       // ◀
            tri << QPointF(x, y - sideA / 2) << QPointF(x - sideA, y) << QPointF(x, y + sideA / 2);

        p.setPen(Qt::NoPen);
        p.setBrush(kLight);  // 白色常显
        p.drawPolygon(tri);
        if (highlight) {     // 条件高亮（绿色覆盖）
            p.setBrush(kGreen);
            p.drawPolygon(tri);
        }
    };

    // —— 工具：画中线小三角（▲ / ▼），白色底 + 条件绿色盖一层 ——
    auto drawCenterArrow = [&](int baseX, int baseY, bool up, bool highlight)
    {
        QPolygonF tri;
        if (up) { // ▲（底边在 baseY）
            tri << QPointF(baseX,         baseY - hTri)
                << QPointF(baseX - halfW, baseY)
                << QPointF(baseX + halfW, baseY);
        } else {  // ▼（底边在 baseY）
            tri << QPointF(baseX,         baseY + hTri)
                << QPointF(baseX - halfW, baseY)
                << QPointF(baseX + halfW, baseY);
        }
        p.setPen(Qt::NoPen);
        p.setBrush(kLight);  // 白色常显
        p.drawPolygon(tri);
        if (highlight) {     // 条件高亮（绿色覆盖）
            p.setBrush(kGreen);
            p.drawPolygon(tri);
        }
    };

    // —— 计算与文字对齐的几何位置 ——
    // 文字的 top / bottom（基于基线 y 的字体度量）
    QFontMetrics fm(p.font());
    const int topTop      = yTopText - fm.ascent();
    const int topBottom   = yTopText + fm.descent();
    const int midTop      = yMidText - fm.ascent();
    const int midBottom   = yMidText + fm.descent();
    const int botTop      = yBotText - fm.ascent();
    const int botBottom   = yBotText + fm.descent();

    // 外侧三角的竖直对齐位置：与上/下行文字的“视觉中心”对齐
    const int yUpperSide = (topTop + topBottom) / 2;
    const int yLowerSide = (botTop + botBottom) / 2;

    // 中间上下小三角的“底边 y”
    const int upBaseY = midTop    - triangleGap;   // ▲ 的底边在文字顶端上方
    const int dnBaseY = midBottom + triangleGap;   // ▼ 的底边在文字底端下方

    // —— 先画四个侧翼三角（白色常显）并按舵角高亮 ——
    // 左舵（上层）：正→右侧上三角绿；负→左侧上三角绿
    drawSideArrow(sideXLeft,  yUpperSide, false, rudder_port_  < 0);
    drawSideArrow(sideXRight, yUpperSide, true,  rudder_port_  > 0);

    // 右舵（下层）：正→右侧下三角绿；负→左侧下三角绿
    drawSideArrow(sideXLeft,  yLowerSide, false, rudder_stbd_  < 0);
    drawSideArrow(sideXRight, yLowerSide, true,  rudder_stbd_  > 0);

    // —— 再画中间上下小三角（白色常显）并按船速正负高亮 ——
    // 速度正→上三角绿；速度负→下三角绿
    drawCenterArrow(cx, upBaseY, true,  speed_ > 0);
    drawCenterArrow(cx, dnBaseY, false, speed_ < 0);
}
