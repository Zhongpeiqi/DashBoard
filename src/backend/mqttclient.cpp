// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "backend/mqttclient.h"
#include <QDebug>
#include "backend/waveconfig.h"
#include <QDateTime>

MqttClient::MqttClient(QObject *parent)
    : QObject(parent)
    , m_client(new QMqttClient(this))
    , m_autoPublishTimer(new QTimer(this))
    , m_currentWaveConfig{1.0, 5.0, 0.0, 0.1, 1.0}  // 初始化默认值
{
    setupMqttClient();

    // m_autoPublishTimer->setInterval(5000);
    // connect(m_autoPublishTimer, &QTimer::timeout, this, [this]() {
    //     if (m_autoPublishEnabled && isConnected()) {
    //         publishThrusters(40.0, 40.0, 0.17, 0.17);
    //     }
    // });
}

MqttClient::~MqttClient()
{
    if (m_client->state() == QMqttClient::Connected) {
        m_client->disconnectFromHost();
    }
}

void MqttClient::setupMqttClient()
{
    m_client->setHostname(BROKER_HOST);
    m_client->setPort(BROKER_PORT);
    m_client->setClientId(CLIENT_ID);
    m_client->setUsername(MQTT_USERNAME);
    m_client->setPassword(MQTT_PASSWORD);
    m_client->setKeepAlive(60);

    // 连接信号到槽函数
    connect(m_client, &QMqttClient::connected, this, &MqttClient::onConnected);
    connect(m_client, &QMqttClient::disconnected, this, &MqttClient::onDisconnected);
    connect(m_client, &QMqttClient::stateChanged, this, &MqttClient::onStateChanged);
    connect(m_client, &QMqttClient::messageReceived, this, &MqttClient::onMessageReceived);
    connect(m_client, &QMqttClient::pingResponseReceived, this, &MqttClient::onPingResponseReceived);
}

void MqttClient::connectToBroker()
{
    if (m_client->state() == QMqttClient::Disconnected) {
        qDebug() << "正在连接到MQTT代理..." << BROKER_HOST << ":" << BROKER_PORT;
        m_client->connectToHost();
    }
}

void MqttClient::setCurrentBoat(const QString &boatName)
{
    if (boatName == m_currentBoat) {
        qDebug() << "船名未变化，跳过切换:" << boatName;
        return; // 船名未变化
    }

    QString oldBoat = m_currentBoat;
    m_currentBoat = boatName;

    qDebug() << "船名已更改:" << oldBoat << "->" << m_currentBoat;

    // 如果已连接，重新订阅所有主题
    if (isConnected()) {
        qDebug() << "重新订阅主题，新主题前缀: vrx/" << m_currentBoat;
        unsubscribeFromAll();
        subscribeToAllSensors();
    } else {
        qDebug() << "MQTT未连接，船只切换将在连接后生效";
    }
}

void MqttClient::disconnectFromBroker()
{
    if (m_client->state() == QMqttClient::Connected) {
        m_client->disconnectFromHost();
    }
}

bool MqttClient::isConnected() const
{
    return m_client->state() == QMqttClient::Connected;
}

QString MqttClient::getCurrentBoat() const
{
    return m_currentBoat;
}

// 新增方法：构建完整主题名
QString MqttClient::buildTopic(const QString &topicTemplate) const
{
    return topicTemplate.arg(m_currentBoat);
}

void MqttClient::subscribeToAllSensors()
{
    subscribeToTopic(buildTopic(TOPIC_IMU_TEMPLATE));
    subscribeToTopic(buildTopic(TOPIC_GPS_TEMPLATE));
    subscribeToTopic(buildTopic(TOPIC_SPEED_TEMPLATE));
    subscribeToTopic(TOPIC_ENVIRONMENT_WIND);
}


void MqttClient::subscribeToTopic(const QString &topic)
{
    if (!isConnected()) {
        emit errorOccurred("未连接到MQTT代理，无法订阅主题");
        return;
    }

    auto subscription = m_client->subscribe(topic, 0);
    if (!subscription) {
        emit errorOccurred("订阅主题失败: " + topic);
        return;
    }

    if (!m_subscribedTopics.contains(topic)) {
        m_subscribedTopics.append(topic);
    }

    qDebug() << "已订阅主题:" << topic;

    // 监控订阅状态
    connect(subscription, &QMqttSubscription::stateChanged, [this, topic](QMqttSubscription::SubscriptionState state) {
        QString stateStr;
        switch (state) {
        case QMqttSubscription::Unsubscribed: stateStr = "未订阅"; break;
        case QMqttSubscription::SubscriptionPending: stateStr = "订阅中"; break;
        case QMqttSubscription::Subscribed: stateStr = "已订阅"; break;
        case QMqttSubscription::Error: stateStr = "订阅错误"; break;
        default: stateStr = "未知状态"; break;
        }
        // qDebug() << "主题" << topic << "订阅状态:" << stateStr;
    });
}

void MqttClient::unsubscribeFromTopic(const QString &topic)
{
    if (isConnected()) {
        m_client->unsubscribe(topic);
        m_subscribedTopics.removeAll(topic);
        qDebug() << "已取消订阅主题:" << topic;
    }
}

void MqttClient::unsubscribeFromAll()
{
    for (const QString &topic : m_subscribedTopics) {
        unsubscribeFromTopic(topic);
    }
}

void MqttClient::publishThrusters(double leftThrust, double rightThrust, double leftPos, double rightPos)
{
    QJsonObject jsonObject;
    jsonObject["left_thrust"] = leftThrust;
    jsonObject["right_thrust"] = rightThrust;
    jsonObject["left_pos"] = leftPos;
    jsonObject["right_pos"] = rightPos;

    QJsonDocument doc(jsonObject);
    QByteArray message = doc.toJson(QJsonDocument::Compact);

    publishMessage(buildTopic(TOPIC_THRUSTERS_TEMPLATE), message);
}

void MqttClient::publishControlStatus(const QString& states)
{
    QJsonObject jsonObject;
    jsonObject["states"] = states;
    QJsonDocument doc(jsonObject);
    QByteArray message = doc.toJson(QJsonDocument::Compact);

    publishMessage(buildTopic(TOPIC_CONTROL_STATUS), message);
}

void MqttClient::publishWaveConfig(const WaveConfig& config)
{
    publishWaveConfigToTopic(TOPIC_ENVIRONMENT_WAVE, config);
    emit waveConfigPublished(config);  // 确保发射信号通知UI
}


void MqttClient::publishWaveConfigToTopic(const QString& topic, const WaveConfig& config)
{
    QJsonObject waveConfig;
    waveConfig["amplitude"] = config.amplitude;
    waveConfig["period"] = config.period;
    waveConfig["direction"] = config.direction;
    waveConfig["steepness"] = config.steepness;
    waveConfig["gain"] = config.gain;
    waveConfig["timestamp"] = QDateTime::currentMSecsSinceEpoch();
    QJsonDocument doc(waveConfig);
    QByteArray message = doc.toJson(QJsonDocument::Compact);
    publishMessage(TOPIC_ENVIRONMENT_WAVE,message);
}

// 修改publishWaveEnvironment实现
void MqttClient::publishWaveEnvironment(const WaveConfig& config)
{
    m_currentWaveConfig = config;

    QJsonObject waveData = config.toJson();
    QJsonDocument doc(waveData);
    QByteArray message = doc.toJson(QJsonDocument::Compact);
    publishMessage(TOPIC_ENVIRONMENT_WAVE, message);

    // qDebug() << "波浪环境配置已发布:"
    //          << "幅值:" << config.amplitude << "m,"
    //          << "周期:" << config.period << "s,"
    //          << "方向:" << config.direction << "rad,"
    //          << "陡峭度:" << config.steepness << ","
    //          << "增益:" << config.gain;

    emit waveConfigChanged(config);
}


void MqttClient::publishMessage(const QString &topic, const QByteArray &message, quint8 qos)
{
    if (!isConnected()) {
        emit errorOccurred("未连接到MQTT代理，无法发布消息");
        return;
    }

    if (m_client->publish(topic, message, qos) == -1) {
        emit errorOccurred("发布消息失败: " + topic);
        return;
    }

    // qDebug() << "已发布消息到主题" << topic << ":" << message;
}

QString MqttClient::connectionState() const
{
    switch (m_client->state()) {
    case QMqttClient::Disconnected: return "已断开";
    case QMqttClient::Connecting: return "连接中";
    case QMqttClient::Connected: return "已连接";
    default: return "未知状态";
    }
}

QList<QString> MqttClient::subscribedTopics() const
{
    return m_subscribedTopics;
}

void MqttClient::onConnected()
{
    qDebug() << "成功连接到MQTT代理";
    emit connectionStateChanged("已连接");

    // 自动订阅所有传感器主题
    subscribeToAllSensors();

    // 启动自动发布
    m_autoPublishEnabled = true;
    m_autoPublishTimer->start();
}

void MqttClient::onDisconnected()
{
    qDebug() << "与MQTT代理的连接已断开";
    emit connectionStateChanged("已断开");
    m_autoPublishTimer->stop();
    m_subscribedTopics.clear();
}

void MqttClient::onStateChanged(QMqttClient::ClientState state)
{
    QString stateStr = connectionState();
    qDebug() << "MQTT连接状态变化:" << stateStr;
    emit connectionStateChanged(stateStr);
}

void MqttClient::onMessageReceived(const QByteArray &message, const QMqttTopicName &topic)
{
    // 1. 记录带时间戳的日志
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    QString logMessage = QString("[%1] 主题: %2 消息: %3")
                             .arg(timestamp)
                             .arg(topic.name())
                             .arg(QString::fromUtf8(message));

    // qDebug().noquote() << logMessage;

    // 发射原始消息信号，通知其他对象"有新的MQTT消息到达了"
    emit messageReceived(topic.name(), message);

    // 处理传感器数据
    processSensorData(topic.name(), message);
}

void MqttClient::onPingResponseReceived()
{
    qDebug() << "Ping响应接收:" << QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
}

void MqttClient::processSensorData(const QString &topic, const QByteArray &message)
{
    QJsonObject data = parseJsonMessage(message);
        if (data.isEmpty()) {
            return;
        }

        // 构建当前船名的完整主题进行比较
        QString imuTopic = buildTopic(TOPIC_IMU_TEMPLATE);
        QString gpsTopic = buildTopic(TOPIC_GPS_TEMPLATE);
        QString speedTopic = buildTopic(TOPIC_SPEED_TEMPLATE);

        QString sensorType;
        if (topic == imuTopic) {
            sensorType = "IMU";
            emit imuDataReceived(data);
        } else if (topic == gpsTopic) {
            sensorType = "GPS";
            emit gpsDataReceived(data);
        } else if (topic == speedTopic) {
            sensorType = "Speed";
            emit speedDataReceived(data);
        } else if (topic == TOPIC_ENVIRONMENT_WIND) {
            sensorType = "Environment";
            emit environmentDataReceived(data);
        } else {
            return;
        }

        emit sensorDataReceived(sensorType, data);
}

QJsonObject MqttClient::parseJsonMessage(const QByteArray &message)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(message, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON解析错误:" << parseError.errorString();
        return QJsonObject();
    }

    return doc.object();
}
