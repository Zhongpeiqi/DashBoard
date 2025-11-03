#ifndef RUDDERCONTROLLER_H
#define RUDDERCONTROLLER_H

#include <QThread>
#include <QVector>
#include <QMutex>

/**
 * @brief 表示左右摇杆的输入状态
 */
struct RudderState {
    float leftX = 0.0f;
    float leftY = 0.0f;
    float rightX = 0.0f;
    float rightY = 0.0f;

    // 为了方便调试输出
    QString toString() const {
        return QString("LX:%1  LY:%2  RX:%3  RY:%4")
                .arg(leftX, 6, 'f', 2)
                .arg(leftY, 6, 'f', 2)
                .arg(rightX, 6, 'f', 2)
                .arg(rightY, 6, 'f', 2);
    }

    // 判断是否有有效输入
    bool hasInput() const {
        return !(qFuzzyIsNull(leftX) && qFuzzyIsNull(leftY) &&
                 qFuzzyIsNull(rightX) && qFuzzyIsNull(rightY));
    }
};

/**
 * @brief 手柄控制器类 - 实时检测摇杆输入
 */
class RudderController : public QThread
{
    Q_OBJECT

public:
    explicit RudderController(QObject *parent = nullptr);
    ~RudderController();

    // 获取当前手柄状态（线程安全）
    RudderState getCurrentState();

    // 获取控制器连接状态
    bool isControllerConnected() const;

    // 停止检测线程
    void stopDetection();

signals:
    // 手柄状态更新信号
    void rudderUpdated(const RudderState &state);
    // 控制器连接状态变化信号
    void controllerStatusChanged(bool connected);

protected:
    void run() override;

private:
    // 当前手柄状态
    RudderState m_currentState;
    // 线程安全互斥锁
    QMutex m_mutex;
    // 运行标志
    volatile bool m_running;
    // 控制器连接状态
    bool m_controllerConnected;

    // 更新状态（线程安全）
    void updateState(const RudderState &newState);
};

#endif // RUDDERCONTROLLER_H
