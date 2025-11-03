#include "wind_roll/wind_rose_widget.h"   // 对应头文件声明
#include <QPainter>                  // QPainter：2D绘图类（用于画刻度、圆、指针等）
#include <QtMath>                    // 提供三角函数与角度弧度转换函数

// =============================
// 构造函数
// =============================
WindRoseWidget::WindRoseWidget(QWidget* parent) : QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent);  // 禁止父控件擦除背景，防止闪烁
    // setMinimumSize(200, 200);               // 设置最小尺寸，避免组件太小重叠
}

// =============================
// 极坐标点计算：角度（从正上方顺时针）→ 圆上的点
// c：圆心；r：半径；deg_from_top_cw：从上方顺时针角度（°）
// 返回：圆上点的坐标
// =============================
QPointF WindRoseWidget::polarPoint(const QPointF& c, double r, double deg_from_top_cw)
{
    double rad = qDegreesToRadians(deg_from_top_cw);           // 角度→弧度
    // Qt 坐标系：x 向右为正，y 向下为正；以顶部为 0° 顺时针为正方向
    return { c.x() + r * std::sin(rad), c.y() - r * std::cos(rad) };
}

// =============================
// 绘制刻度环（包含外圈圆与数字刻度）
// =============================
void WindRoseWidget::drawScale(QPainter& p, const QRectF& rc, double r, double& scale_outer_radius)
{
    p.save();                                                  // 保存画笔状态
    const double r_inner = r;                                  // 圆基线（刻度起点）
    const double r_minor = r * 1.05;                           // 小刻度外端
    const double r_major = r * 1.10;                           // 大刻度外端
    scale_outer_radius = r_major;                              // 记录最外刻度半径

    p.setRenderHint(QPainter::Antialiasing, true);             // 抗锯齿使线条平滑

    // 每 5° 一小刻度，30° 一大刻度
    for (int a = 0; a < 360; a += 5) {
        bool major = (a % 30 == 0);                            // 每 30° 为大刻度
        p.setPen(QPen(QColor(200, 210, 220), major ? 2 : 1));  // 区分线宽

        // 刻度线起点与终点
        QPointF A = polarPoint(rc.center(), r_inner, a);
        QPointF B = polarPoint(rc.center(), major ? r_major : r_minor, a);
        p.drawLine(A, B);

        // 绘制角度数字（仅大刻度）
        if (major) {
            int label_val = a;                                 // 直接显示角度值
            QPointF T = polarPoint(rc.center(), r * 1.22, a);  // 数字位置（在圆外）
            QRectF tr(T.x() - 20, T.y() - 14, 40, 28);         // 文本矩形

            QFont f = p.font(); f.setPointSize(12); f.setBold(true);
            p.setFont(f);
            p.setPen(Qt::white);
            p.drawText(tr, Qt::AlignCenter, QString::number(label_val)); // 居中绘制数字
        }
    }

    // 绘制外圈圆
    p.setPen(QPen(QColor(150, 160, 170), 2));                   // 外圈浅灰色
    p.drawEllipse(rc.center(), r, r);                           // 画主圆
    p.restore();                                                // 恢复画笔状态
}

// =============================
// 绘制倒三角指针
// 参数说明：
//   c —— 圆心
//   r —— 半径
//   deg_from_top_cw —— 从上方顺时针角度（指针方向）
//   color —— 指针颜色
// =============================
void WindRoseWidget::drawPointerTriangle(QPainter& p, const QPointF& c, double r, double deg_from_top_cw, const QColor& color)
{
    QPointF tip = polarPoint(c, r * 0.82, deg_from_top_cw);     // 尖端点（靠外侧 82% 半径）
    double rad = qDegreesToRadians(deg_from_top_cw);            // 转为弧度

    // 定义方向向量
    QPointF tangent(std::cos(rad), std::sin(rad));              // 切线方向
    QPointF inward(-std::sin(rad), std::cos(rad));              // 向内方向（指向圆心）

    double base_half = r * 0.08;                                // 底边半宽
    double height    = r * 0.14;                                // 高度（尖端→底中心）

    // 底边中心
    QPointF baseC = tip - inward * height;
    QPointF L = baseC - tangent * base_half;
    QPointF R = baseC + tangent * base_half;

    QPolygonF tri; tri << tip << L << R;

    p.save();
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(Qt::NoPen);
    p.setBrush(color);
    p.drawPolygon(tri);                                         // 绘制指针
    p.restore();
}

// =============================
// 主绘制函数：整个风向盘绘制入口
// =============================
void WindRoseWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.fillRect(rect(), QColor(0, 0, 0));                       // 黑色背景（仪表风格）

    // 自适应边距（留出绘图空隙）
    double margin = qMin(width(), height()) * 0.1452;
    QRectF rc = rect().adjusted(margin - 10, margin, -margin + 20, -margin);

    // 圆半径按比例计算 —— 这是控制风向盘大小的核心参数
    double r = qMin(rc.width(), rc.height()) * 0.44;

    double outer_r = 0.0;
    drawScale(p, rc, r, outer_r);                              // 绘制刻度环

    // 中心点（视觉中心）
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setBrush(Qt::white);
    p.setPen(Qt::NoPen);
    p.drawEllipse(rc.center(), 4, 4);

    // 指针（可扩展为多层）
    drawPointerTriangle(p, rc.center(), r, true_dir_deg_, trueColor_); // 主风向
}
