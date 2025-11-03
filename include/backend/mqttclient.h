#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <QObject>
#include <QMqttClient>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTimer>
#include "waveconfig.h"

class MqttClient : public QObject
{
    Q_OBJECT

public:
    explicit MqttClient(QObject *parent = nullptr);
    ~MqttClient();

    // 连接管理
    void connectToBroker();
    void disconnectFromBroker();
    bool isConnected() const;

    // 船名管理
    void setCurrentBoat(const QString &boatName);
    QString getCurrentBoat() const;

    // 订阅管理
    void subscribeToAllSensors();
    void subscribeToTopic(const QString &topic);
    void unsubscribeFromTopic(const QString &topic);
    void unsubscribeFromAll();

    // 发布消息
    void publishThrusters(double leftThrust, double rightThrust, double leftPos, double rightPos);
    void publishControlStatus(const QString &states);
    void publishMessage(const QString &topic, const QByteArray &message, quint8 qos = 0);
    void publishWaveConfig(const WaveConfig& config);
    // 修改方法签名，使用WaveConfig而不是QJsonObject
    void publishWaveEnvironment(const WaveConfig& config);

    // 可选：保留旧的QJsonObject版本作为重载（如果需要向后兼容）
    void publishWaveEnvironment(const QJsonObject& waveData) {
        WaveConfig config;
        config.fromJson(waveData);
        publishWaveEnvironment(config);
    }

    // 添加获取当前配置的方法
    WaveConfig getCurrentWaveConfig() const { return m_currentWaveConfig; }

    // 获取状态信息
    QString connectionState() const;
    QList<QString> subscribedTopics() const;


signals:
    // 状态信号
    void connectionStateChanged(const QString &state);
    void messageReceived(const QString &topic, const QByteArray &message);
    void sensorDataReceived(const QString &sensorType, const QJsonObject &data);
    void errorOccurred(const QString &error);

    // 特定传感器数据信号
    void imuDataReceived(const QJsonObject &data);
    void gpsDataReceived(const QJsonObject &data);
    void speedDataReceived(const QJsonObject &data);
    void environmentDataReceived(const QJsonObject &data);
    // 船只切换信号
    void boatChanged(const QString& boatName);
    // 添加波浪配置变化信号
    void waveConfigChanged(const WaveConfig& config);
    void waveConfigPublished(const WaveConfig& config);

private slots:
    void onConnected();
    void onDisconnected();
    void onStateChanged(QMqttClient::ClientState state);
    void onMessageReceived(const QByteArray &message, const QMqttTopicName &topic);
    void onPingResponseReceived();

private:
    void setupMqttClient();
    void processSensorData(const QString &topic, const QByteArray &message);
    QJsonObject parseJsonMessage(const QByteArray &message);
    QString buildTopic(const QString &topicTemplate) const;
    void publishWaveConfigToTopic(const QString& topic, const WaveConfig& config);

    QMqttClient *m_client;
    QTimer *m_autoPublishTimer;
    // 添加波浪配置相关成员变量
    WaveConfig m_currentWaveConfig;

    // MQTT配置
    const QString BROKER_HOST = "60.205.13.156";
    const quint16 BROKER_PORT = 1883;
    const QString CLIENT_ID = "boat-control-backend";
    const QString MQTT_USERNAME = "Drone/1hbahhhckc4ky";
    const QString MQTT_PASSWORD = "tonzajuxhvmr";

    // 主题模板定义 (使用占位符)
    const QString TOPIC_IMU_TEMPLATE = "vrx/%1/sensors/imu";
    const QString TOPIC_GPS_TEMPLATE = "vrx/%1/sensors/gps";
    const QString TOPIC_SPEED_TEMPLATE = "vrx/%1/sensors/speed";
    const QString TOPIC_ENVIRONMENT_WAVE = "vrx/environment/waves";
    const QString TOPIC_ENVIRONMENT_WIND = "vrx/environment/sensors/wind";
    const QString TOPIC_THRUSTERS_TEMPLATE = "vrx/%1/control/thrusters";
    const QString TOPIC_CONTROL_STATUS = "vrx/%1/control/status";

    QString m_currentBoat = "boat1"; // 默认船名

    QList<QString> m_subscribedTopics;
    bool m_autoPublishEnabled = false;
};

#endif // MQTTCLIENT_H
