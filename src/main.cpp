#include <QApplication>
#include <QTimer>
#include <QDebug>
#include <QObject>
#include <QThread>
#include <QMetaObject>
#include <QMutex>
#include <QMutexLocker>
#include <QDateTime>
#include <cmath>

#include "backend/mqttclient.h"
#include "backend/ruddercontroller.h"
#include "main_window.h"
#include <SDL2/SDL.h>

class ControllerBridge : public QObject
{
    Q_OBJECT

public:
    // 修改构造函数，接受外部的MQTT客户端实例
    explicit ControllerBridge(MqttClient* mqttClient, QObject *parent = nullptr)
        : QObject(parent)
        , m_mqttClient(mqttClient)  // 使用外部传入的MQTT客户端
        , m_rudderController(new RudderController(this))
    {
        // 连接手柄状态更新信号
        connect(m_rudderController, &RudderController::rudderUpdated,
                this, &ControllerBridge::onRudderUpdated);

        // 连接手柄连接状态变化信号
        connect(m_rudderController, &RudderController::controllerStatusChanged,
                this, &ControllerBridge::onControllerStatusChanged);

        // 连接MQTT状态信号
        connect(m_mqttClient, &MqttClient::connectionStateChanged,
                this, &ControllerBridge::onMqttStateChanged);

        // 启动手柄检测线程
        m_rudderController->start();

        qDebug() << "控制器桥接器初始化完成";
    }

    ~ControllerBridge()
    {
        m_rudderController->stopDetection();
        qDebug() << "控制器桥接器已销毁";
    }

    // 设置MainWindow引用
    void setMainWindow(MainWindow *window) {
        m_mainWindow = window;
    }

    // 获取MQTT客户端实例
    MqttClient* getMqttClient() const {
        return m_mqttClient;
    }

private slots:
    void onRudderUpdated(const RudderState &state)
    {
        // 将摇杆数据映射到推力器控制参数
        double leftThrust = mapAxisToThrust(state.leftY);
        double rightThrust = mapAxisToThrust(state.rightY);
        double leftPos = mapAxisToPosition(state.leftX);
        double rightPos = mapAxisToPosition(state.rightX);

        // 发送推力器控制消息
        if (m_mqttClient->isConnected()) {
            m_mqttClient->publishThrusters(leftThrust, rightThrust, leftPos, rightPos);
        }

        // 调试输出（每20次输出一次以减少日志）
        static int debugCounter = 0;
        // if (debugCounter++ % 20 == 0) {
        //     QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
        //     qDebug().noquote() << QString("[%1] 摇杆: LX:%2 LY:%3 RX:%4 RY:%5")
        //                 .arg(timestamp)
        //                 .arg(state.leftX, 6, 'f', 3)
        //                 .arg(state.leftY, 6, 'f', 3)
        //                 .arg(state.rightX, 6, 'f', 3)
        //                 .arg(state.rightY, 6, 'f', 3);

        //     qDebug().noquote() << QString("      控制: 推力(L:%1 R:%2) 位置(L:%3 R:%4)")
        //                 .arg(leftThrust, 8, 'f', 1)
        //                 .arg(rightThrust, 8, 'f', 1)
        //                 .arg(leftPos, 5, 'f', 3)
        //                 .arg(rightPos, 5, 'f', 3);
        // }

        // 同时更新UI显示（如果设置了MainWindow）
        if (m_mainWindow) {
            updateMainWindowWithJoystickData(state);
        }
    }

    void onControllerStatusChanged(bool connected)
    {
        if (connected) {
            qDebug() << "手柄控制器已连接";
        } else {
            qWarning() << "手柄控制器断开连接";
        }
    }

    void onMqttStateChanged(const QString &state)
    {
        qDebug() << "MQTT连接状态:" << state;

        // 如果MQTT断开连接，尝试重新连接
        if (state == "已断开") {
            QTimer::singleShot(3000, m_mqttClient, &MqttClient::connectToBroker);
        }
    }

private:
    // 将摇杆轴值映射到推力范围 [-1000, 1000]
    double mapAxisToThrust(float axisValue)
    {
        // 添加死区处理，避免摇杆微小移动造成的误操作
        if (qAbs(axisValue) < 0.05f) {
            return 0.0;
        }
        return axisValue * 1000.0;
    }

    // 将摇杆轴值映射到位置范围 [-1, 1]
    double mapAxisToPosition(float axisValue)
    {
        // 添加死区处理
        if (qAbs(axisValue) < 0.05f) {
            return 0.0;
        }
        return axisValue;
    }

    // 将摇杆水平值映射到舵角范围（-40°到40°）
    double mapToRudderAngle(double joystickValue)
    {
        return joystickValue * 40.0;
    }

    // 使用摇杆数据更新主窗口
    void updateMainWindowWithJoystickData(const RudderState &state)
    {
        if (!m_mainWindow) return;

        // 处理左摇杆（port）
        double portOrder = state.leftY * 1000.0;
        double portRudder = state.leftX;

        // 处理右摇杆（stbd）
        double stbdOrder = state.rightY * 1000.0;
        double stbdRudder = state.rightX;

        // 更新界面显示
        m_mainWindow->updateEngines(portOrder/10, mapToRudderAngle(portRudder),
                                   stbdOrder/10, mapToRudderAngle(stbdRudder));
    }

    MqttClient *m_mqttClient;        // 使用外部传入的MQTT客户端
    RudderController *m_rudderController;
    MainWindow *m_mainWindow = nullptr;
};

class SensorDataBridge : public QObject
{
    Q_OBJECT

public:
    // 修改构造函数，接受外部的MQTT客户端实例
    explicit SensorDataBridge(MqttClient* mqttClient, QObject *parent = nullptr)
        : QObject(parent)
        , m_mqttClient(mqttClient)  // 使用外部传入的MQTT客户端
    {
        // 连接MQTT状态信号
        connect(m_mqttClient, &MqttClient::connectionStateChanged,
                this, &SensorDataBridge::onMqttStateChanged);

        // 连接传感器数据信号 - 使用QueuedConnection确保在主线程执行
        connect(m_mqttClient, &MqttClient::imuDataReceived,
                this, &SensorDataBridge::onImuDataReceived, Qt::QueuedConnection);
        connect(m_mqttClient, &MqttClient::gpsDataReceived,
                this, &SensorDataBridge::onGpsDataReceived, Qt::QueuedConnection);
        connect(m_mqttClient, &MqttClient::speedDataReceived,
                this, &SensorDataBridge::onSpeedDataReceived, Qt::QueuedConnection);
        connect(m_mqttClient, &MqttClient::environmentDataReceived,
                this, &SensorDataBridge::onEnvironmentDataReceived, Qt::QueuedConnection);

        // 连接通用传感器数据信号（备用）
        connect(m_mqttClient, &MqttClient::sensorDataReceived,
                this, &SensorDataBridge::onSensorDataReceived, Qt::QueuedConnection);

        qDebug() << "传感器数据桥接器初始化完成，等待MQTT数据...";
    }

    ~SensorDataBridge()
    {
        qDebug() << "传感器数据桥接器已销毁";
    }

    // 设置MainWindow引用
    void setMainWindow(MainWindow *window) {
        m_mainWindow = window;
    }

    // 获取当前传感器数据
    struct SensorData {
        // IMU数据
        double roll = 0.0;
        double pitch = 0.0;
        double yaw = 0.0;

        // 角速度数据
        double angular_velocity_x = 0.0;
        double angular_velocity_y = 0.0;
        double angular_velocity_z = 0.0;

        // 线性加速度数据
        double acceleration_x = 0.0;
        double acceleration_y = 0.0;
        double acceleration_z = 0.0;

        // 计算出的额外数据
        double acceleration_magnitude = 0.0;
        double angular_velocity_magnitude = 0.0;
        double gravity_projection = 0.0;

        // GPS数据
        double latitude = 0.0;
        double longitude = 0.0;
        double altitude = 0.0;
        double cog = 0.0;

        // 速度数据
        double speed_ms = 0.0;
        double speed_knots = 0.0;

        // 环境数据
        double wind_direction_rad = 0.0;
        double wind_speed = 0.0;
        double wind_direction_deg = 0.0;

        // 时间戳
        qint64 lastUpdate = 0;
        qint64 lastImuUpdate = 0;

        // 历史数据
        double prev_roll = 0.0;
        double prev_pitch = 0.0;
        double prev_yaw = 0.0;
        double roll_rate = 0.0;
        double pitch_rate = 0.0;
        double yaw_rate = 0.0;
    };

    SensorData getCurrentData() const {
        QMutexLocker locker(&m_dataMutex);
        return m_currentData;
    }

private slots:
    void onMqttStateChanged(const QString &state)
    {
        qDebug() << "MQTT连接状态:" << state;

        // 如果MQTT断开连接，尝试重新连接
        if (state == "已断开") {
            QTimer::singleShot(3000, m_mqttClient, &MqttClient::connectToBroker);
        }
    }

    void onImuDataReceived(const QJsonObject &data)
    {
        QMutexLocker locker(&m_dataMutex);

        // 解析四元数数据并转换为欧拉角
        QJsonObject orientation = data.value("orientation").toObject();
        double x = orientation.value("x").toDouble(0.0);
        double y = orientation.value("y").toDouble(0.0);
        double z = orientation.value("z").toDouble(0.0);
        double w = orientation.value("w").toDouble(1.0);

        double roll, pitch, yaw;
        double sinr_cosp = 2.0 * (w * x + y * z);
        double cosr_cosp = 1.0 - 2.0 * (x * x + y * y);
        roll = std::atan2(sinr_cosp, cosr_cosp);

        double sinp = 2.0 * (w * y - z * x);
        if (std::abs(sinp) >= 1.0) {
            pitch = std::copysign(M_PI / 2.0, sinp);
        } else {
            pitch = std::asin(sinp);
        }

        double siny_cosp = 2.0 * (w * z + x * y);
        double cosy_cosp = 1.0 - 2.0 * (y * y + z * z);
        yaw = std::atan2(siny_cosp, cosy_cosp);

        // 转换为角度制
        roll = roll * 180.0 / M_PI;
        pitch = pitch * 180.0 / M_PI;
        yaw = yaw * 180.0 / M_PI;

        // 解析角速度数据
        QJsonObject angularVelocity = data.value("angular_velocity").toObject();
        double ang_vel_x = angularVelocity.value("x").toDouble(0.0);
        double ang_vel_y = angularVelocity.value("y").toDouble(0.0);
        double ang_vel_z = angularVelocity.value("z").toDouble(0.0);

        // 解析线性加速度数据
        QJsonObject linearAcceleration = data.value("linear_acceleration").toObject();
        double accel_x = linearAcceleration.value("x").toDouble(0.0);
        double accel_y = linearAcceleration.value("y").toDouble(0.0);
        double accel_z = linearAcceleration.value("z").toDouble(0.0);

        // 计算额外数据
        qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
        qint64 deltaTime = 0;
        if (m_currentData.lastImuUpdate > 0) {
            deltaTime = currentTime - m_currentData.lastImuUpdate;
        }

        m_currentData.acceleration_magnitude = std::sqrt(
            accel_x * accel_x + accel_y * accel_y + accel_z * accel_z);

        m_currentData.angular_velocity_magnitude = std::sqrt(
            ang_vel_x * ang_vel_x + ang_vel_y * ang_vel_y + ang_vel_z * ang_vel_z);

        m_currentData.gravity_projection = accel_z;

        // 计算姿态角变化率
        if (deltaTime > 0 && m_currentData.lastImuUpdate > 0) {
            double deltaTimeSeconds = deltaTime / 1000.0;
            m_currentData.roll_rate = (roll - m_currentData.prev_roll) / deltaTimeSeconds;
            m_currentData.pitch_rate = (pitch - m_currentData.prev_pitch) / deltaTimeSeconds;
            m_currentData.yaw_rate = (yaw - m_currentData.prev_yaw) / deltaTimeSeconds;
        }

        // 保存历史数据
        m_currentData.prev_roll = roll;
        m_currentData.prev_pitch = pitch;
        m_currentData.prev_yaw = yaw;

        // 更新当前数据
        m_currentData.roll = roll;
        m_currentData.pitch = pitch;
        m_currentData.yaw = yaw;
        m_currentData.angular_velocity_x = ang_vel_x;
        m_currentData.angular_velocity_y = ang_vel_y;
        m_currentData.angular_velocity_z = ang_vel_z;
        m_currentData.acceleration_x = accel_x;
        m_currentData.acceleration_y = accel_y;
        m_currentData.acceleration_z = accel_z;
        m_currentData.lastUpdate = currentTime;
        m_currentData.lastImuUpdate = currentTime;

        // qDebug() << "IMU数据更新 - 欧拉角(度) - 横滚:" << roll
        //          << "俯仰:" << pitch
        //          << "偏航:" << yaw;

        QMetaObject::invokeMethod(this, "updateMainWindow", Qt::QueuedConnection);
    }

    void onGpsDataReceived(const QJsonObject &data)
    {
        QMutexLocker locker(&m_dataMutex);

        m_currentData.latitude = data.value("latitude").toDouble(0.0);
        m_currentData.longitude = data.value("longitude").toDouble(0.0);
        m_currentData.altitude = data.value("altitude").toDouble(0.0);
        m_currentData.cog = data.value("status").toDouble(0.0);
        m_currentData.lastUpdate = QDateTime::currentMSecsSinceEpoch();

        // qDebug() << "GPS数据更新 - 纬度:" << m_currentData.latitude
        //          << "经度:" << m_currentData.longitude
        //          << "海拔:" << m_currentData.altitude
        //          << "航向:" << m_currentData.cog;

        QMetaObject::invokeMethod(this, "updateMainWindow", Qt::QueuedConnection);
    }

    void onSpeedDataReceived(const QJsonObject &data)
    {
        QMutexLocker locker(&m_dataMutex);

        m_currentData.speed_ms = data.value("speed_ms").toDouble(0.0);
        m_currentData.speed_knots = data.value("speed_knots").toDouble(0.0);
        m_currentData.lastUpdate = QDateTime::currentMSecsSinceEpoch();

        // qDebug() << "速度数据更新 - 速度(m/s):" << m_currentData.speed_ms
        //          << "速度(节):" << m_currentData.speed_knots;

        QMetaObject::invokeMethod(this, "updateMainWindow", Qt::QueuedConnection);
    }

    void onEnvironmentDataReceived(const QJsonObject &data)
    {
        QMutexLocker locker(&m_dataMutex);

        m_currentData.wind_speed = data.value("wind_speed").toDouble(0.0);
        m_currentData.wind_direction_deg = data.value("wind_direction_deg").toDouble(0.0);
        m_currentData.wind_direction_rad = data.value("wind_direction_rad").toDouble(0.0);
        m_currentData.lastUpdate = QDateTime::currentMSecsSinceEpoch();

        // qDebug() << "环境数据更新 - 风速:" << m_currentData.wind_speed
        //          << "风向(度):" << m_currentData.wind_direction_deg;

        QMetaObject::invokeMethod(this, "updateMainWindow", Qt::QueuedConnection);
    }

    void onSensorDataReceived(const QString &sensorType, const QJsonObject &data)
    {
        qDebug() << "通用传感器数据:" << sensorType << data;
    }

private slots:
    void updateMainWindow()
    {
        if (!m_mainWindow) return;

        QMutexLocker locker(&m_dataMutex);

        qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
        if (currentTime - m_currentData.lastUpdate > 5000) {
            qDebug() << "传感器数据已过时，不使用";
            return;
        }

        // 使用真实数据更新UI
        m_mainWindow->updateWind(m_currentData.wind_direction_rad,
                                m_currentData.wind_speed,
                                m_currentData.roll);

        double cog = m_currentData.cog;
        if (cog == 0.0 && m_currentData.yaw != 0.0) {
            cog = m_currentData.yaw;
        }

        m_mainWindow->updateNav(m_currentData.latitude,
                               m_currentData.longitude,
                               m_currentData.altitude,
                               cog,
                               m_currentData.speed_knots,
                               m_currentData.pitch);

        m_mainWindow->updatePropulsion(m_currentData.acceleration_magnitude,
                                      m_currentData.speed_knots,
                                      m_currentData.acceleration_magnitude);
    }

private:
    MqttClient *m_mqttClient;        // 使用外部传入的MQTT客户端
    MainWindow *m_mainWindow = nullptr;
    SensorData m_currentData;
    mutable QMutex m_dataMutex;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow w;
    w.setWindowTitle("Ship Dashboard Control Center - Real Data Mode");
    w.show();

    // 创建MQTT客户端实例
    MqttClient* mqttClient = new MqttClient(&app);
    mqttClient->connectToBroker();

    // 创建控制器桥接器
    ControllerBridge controllerBridge(mqttClient);
    controllerBridge.setMainWindow(&w);

    // 创建传感器数据桥接器
    SensorDataBridge sensorBridge(mqttClient);
    sensorBridge.setMainWindow(&w);

    // 连接船只切换信号
    QObject::connect(&w, &MainWindow::sendBoatSelectionToMqtt,
                     mqttClient, &MqttClient::setCurrentBoat);

    // +++ 修复：添加调试输出，确保信号连接正确 +++
    // qDebug() << "开始连接波浪配置信号...";

    // 波浪配置信号连接 - 添加类型转换明确函数指针
    bool waveConnect = QObject::connect(&w, &MainWindow::sendWaveConfigToMqtt,
                                        mqttClient,
                                        static_cast<void(MqttClient::*)(const WaveConfig&)>(&MqttClient::publishWaveConfig));

    // 波浪配置发布完成信号连接
    bool wavePublishedConnect = QObject::connect(mqttClient, &MqttClient::waveConfigPublished,
                                                 &w, &MainWindow::onWaveConfigPublished);

    // +++ 新增：连接控制状态信号 +++
    QObject::connect(&w, &MainWindow::sendControlStatusToMqtt,
                        mqttClient, &MqttClient::publishControlStatus);

    int result = app.exec();
    qDebug() << "应用程序退出，返回码:" << result;
    return result;
}

#include "main.moc"
