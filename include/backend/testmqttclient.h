#ifndef TESTMQTTCLIENT_H
#define TESTMQTTCLIENT_H

#include <QObject>
#include "backend/mqttclient.h"

class TestMqttClient : public QObject
{
    Q_OBJECT

public:
    explicit TestMqttClient(QObject *parent = nullptr);
    void runTests();

private slots:
    void onConnectionStateChanged(const QString &state);
    void onMessageReceived(const QString &topic, const QByteArray &message);
    void onSensorDataReceived(const QString &sensorType, const QJsonObject &data);
    void onImuDataReceived(const QJsonObject &data);
    void onGpsDataReceived(const QJsonObject &data);
    void onErrorOccurred(const QString &error);

private:
    MqttClient *m_mqttClient;
};

#endif // TESTMQTTCLIENT_H
