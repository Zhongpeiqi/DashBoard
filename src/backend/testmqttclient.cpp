#include "backend/testmqttclient.h"
#include <QTimer>
#include <QCoreApplication>
#include <QDebug>
#include <QObject>

TestMqttClient::TestMqttClient(QObject *parent)
   : QObject(parent)
   , m_mqttClient(new MqttClient(this))
{
   // 连接信号
   connect(m_mqttClient, &MqttClient::connectionStateChanged,
           this, &TestMqttClient::onConnectionStateChanged);
   connect(m_mqttClient, &MqttClient::messageReceived,
           this, &TestMqttClient::onMessageReceived);
   connect(m_mqttClient, &MqttClient::sensorDataReceived,
           this, &TestMqttClient::onSensorDataReceived);
   connect(m_mqttClient, &MqttClient::imuDataReceived,
           this, &TestMqttClient::onImuDataReceived);
   connect(m_mqttClient, &MqttClient::gpsDataReceived,
           this, &TestMqttClient::onGpsDataReceived);
   connect(m_mqttClient, &MqttClient::errorOccurred,
           this, &TestMqttClient::onErrorOccurred);
}

void TestMqttClient::runTests()
{
   qDebug() << "=== MQTT客户端测试开始 ===";

   // 测试1: 连接MQTT代理
   qDebug() << "测试1: 连接MQTT代理";
   m_mqttClient->connectToBroker();
   QJsonObject waveData;
   waveData["amplitude"] = 2.0;
   waveData["period"] = 8.0;
   waveData["direction"] = 0.0;
   waveData["steepness"] = 0.3;
   waveData["gain"] = 0.7;
   m_mqttClient->publishWaveEnvironment(waveData);
}

void TestMqttClient::onConnectionStateChanged(const QString &state)
{
   qDebug() << "连接状态变化:" << state;
}

void TestMqttClient::onMessageReceived(const QString &topic, const QByteArray &message)
{
   qDebug() << "收到消息 - 主题:" << topic << "内容:" << message;
}

void TestMqttClient::onSensorDataReceived(const QString &sensorType, const QJsonObject &data)
{
   // qDebug() << "传感器数据 - 类型:" << sensorType << "数据:" << QJsonDocument(data).toJson();
}

void TestMqttClient::onImuDataReceived(const QJsonObject &data)
{
   if (data.contains("orientation") && data["orientation"].isObject()) {
       QJsonObject orientation = data["orientation"].toObject();
       if (orientation.contains("euler") && orientation["euler"].isObject()) {
           QJsonObject euler = orientation["euler"].toObject();
           qDebug() << "IMU欧拉角 - 横滚:" << euler["roll"].toDouble()
                    << "俯仰:" << euler["pitch"].toDouble()
                    << "偏航:" << euler["yaw"].toDouble();
       }
   }
}

void TestMqttClient::onGpsDataReceived(const QJsonObject &data)
{
   qDebug() << "GPS坐标 - 纬度:" << data["latitude"].toDouble()
       << "经度:" << data["longitude"].toDouble()
       << "海拔:" << data["altitude"].toDouble() << "米";
}

void TestMqttClient::onErrorOccurred(const QString &error)
{
   qWarning() << "错误发生:" << error;
}
