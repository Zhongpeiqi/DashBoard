#pragma once
#include <QWidget>

class QPainter;

class SpeedLogWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SpeedLogWidget(QWidget* parent = nullptr);

    /**
     * @brief 设置速度显示
     * @param accel  加速度值（显示在上/下两行，相同的一个值）
     * @param speed  当前船速（显示在中间）
     * @param accel_dup  兼容旧接口，忽略或传同 accel 均可
     *
     * 兼容旧调用：speed_log_->setSpeed(bowThrust, mainSpeed, sternThrust);
     * 其中 bowThrust 作为“加速度”，mainSpeed 作为“当前速度”，sternThrust 可忽略。
     */
    void setSpeed(double accel, double speed, double accel_dup);

    /**
     * @brief 设置左右舵角（用于决定左右侧三角的绿色高亮）
     * @param rudder_port  左舵角（正→右侧上三角绿；负→左侧上三角绿）
     * @param rudder_stbd  右舵角（正→右侧下三角绿；负→左侧下三角绿）
     */
    void setRudders(double rudder_port, double rudder_stbd);

protected:
    void paintEvent(QPaintEvent*) override;

private:
    // 数值模型
    double accel_        = 0.0;  // 上/下两行显示的同一个加速度
    double speed_        = 0.0;  // 中间显示的当前船速
    double rudder_port_  = 0.0;  // 左舵角
    double rudder_stbd_  = 0.0;  // 右舵角

    // 封装的三角绘制逻辑（白色常显 + 条件绿色高亮）
    void drawIndicatorTriangles(QPainter& p,
                                int cx,          // 中线 x（船体中心）
                                int yTopText,    // 上行数值的基线 y
                                int yMidText,    // 中间数值的基线 y
                                int yBotText,    // 下行数值的基线 y
                                int bodyWidth);  // 船体宽，用于算外侧三角的 x
};
