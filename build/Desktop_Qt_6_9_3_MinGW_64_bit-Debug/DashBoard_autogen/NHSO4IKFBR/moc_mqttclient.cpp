/****************************************************************************
** Meta object code from reading C++ file 'mqttclient.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../include/backend/mqttclient.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mqttclient.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN10MqttClientE_t {};
} // unnamed namespace

template <> constexpr inline auto MqttClient::qt_create_metaobjectdata<qt_meta_tag_ZN10MqttClientE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "MqttClient",
        "connectionStateChanged",
        "",
        "state",
        "messageReceived",
        "topic",
        "message",
        "sensorDataReceived",
        "sensorType",
        "data",
        "errorOccurred",
        "error",
        "imuDataReceived",
        "gpsDataReceived",
        "speedDataReceived",
        "environmentDataReceived",
        "boatChanged",
        "boatName",
        "waveConfigChanged",
        "WaveConfig",
        "config",
        "waveConfigPublished",
        "onConnected",
        "onDisconnected",
        "onStateChanged",
        "QMqttClient::ClientState",
        "onMessageReceived",
        "QMqttTopicName",
        "onPingResponseReceived"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'connectionStateChanged'
        QtMocHelpers::SignalData<void(const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'messageReceived'
        QtMocHelpers::SignalData<void(const QString &, const QByteArray &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 5 }, { QMetaType::QByteArray, 6 },
        }}),
        // Signal 'sensorDataReceived'
        QtMocHelpers::SignalData<void(const QString &, const QJsonObject &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 }, { QMetaType::QJsonObject, 9 },
        }}),
        // Signal 'errorOccurred'
        QtMocHelpers::SignalData<void(const QString &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 11 },
        }}),
        // Signal 'imuDataReceived'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QJsonObject, 9 },
        }}),
        // Signal 'gpsDataReceived'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QJsonObject, 9 },
        }}),
        // Signal 'speedDataReceived'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QJsonObject, 9 },
        }}),
        // Signal 'environmentDataReceived'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QJsonObject, 9 },
        }}),
        // Signal 'boatChanged'
        QtMocHelpers::SignalData<void(const QString &)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 17 },
        }}),
        // Signal 'waveConfigChanged'
        QtMocHelpers::SignalData<void(const WaveConfig &)>(18, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 19, 20 },
        }}),
        // Signal 'waveConfigPublished'
        QtMocHelpers::SignalData<void(const WaveConfig &)>(21, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 19, 20 },
        }}),
        // Slot 'onConnected'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDisconnected'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onStateChanged'
        QtMocHelpers::SlotData<void(QMqttClient::ClientState)>(24, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 25, 3 },
        }}),
        // Slot 'onMessageReceived'
        QtMocHelpers::SlotData<void(const QByteArray &, const QMqttTopicName &)>(26, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QByteArray, 6 }, { 0x80000000 | 27, 5 },
        }}),
        // Slot 'onPingResponseReceived'
        QtMocHelpers::SlotData<void()>(28, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MqttClient, qt_meta_tag_ZN10MqttClientE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject MqttClient::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MqttClientE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MqttClientE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10MqttClientE_t>.metaTypes,
    nullptr
} };

void MqttClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MqttClient *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->connectionStateChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->messageReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[2]))); break;
        case 2: _t->sensorDataReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QJsonObject>>(_a[2]))); break;
        case 3: _t->errorOccurred((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->imuDataReceived((*reinterpret_cast< std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 5: _t->gpsDataReceived((*reinterpret_cast< std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 6: _t->speedDataReceived((*reinterpret_cast< std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 7: _t->environmentDataReceived((*reinterpret_cast< std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 8: _t->boatChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 9: _t->waveConfigChanged((*reinterpret_cast< std::add_pointer_t<WaveConfig>>(_a[1]))); break;
        case 10: _t->waveConfigPublished((*reinterpret_cast< std::add_pointer_t<WaveConfig>>(_a[1]))); break;
        case 11: _t->onConnected(); break;
        case 12: _t->onDisconnected(); break;
        case 13: _t->onStateChanged((*reinterpret_cast< std::add_pointer_t<QMqttClient::ClientState>>(_a[1]))); break;
        case 14: _t->onMessageReceived((*reinterpret_cast< std::add_pointer_t<QByteArray>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QMqttTopicName>>(_a[2]))); break;
        case 15: _t->onPingResponseReceived(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 13:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QMqttClient::ClientState >(); break;
            }
            break;
        case 14:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QMqttTopicName >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (MqttClient::*)(const QString & )>(_a, &MqttClient::connectionStateChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (MqttClient::*)(const QString & , const QByteArray & )>(_a, &MqttClient::messageReceived, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (MqttClient::*)(const QString & , const QJsonObject & )>(_a, &MqttClient::sensorDataReceived, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (MqttClient::*)(const QString & )>(_a, &MqttClient::errorOccurred, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (MqttClient::*)(const QJsonObject & )>(_a, &MqttClient::imuDataReceived, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (MqttClient::*)(const QJsonObject & )>(_a, &MqttClient::gpsDataReceived, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (MqttClient::*)(const QJsonObject & )>(_a, &MqttClient::speedDataReceived, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (MqttClient::*)(const QJsonObject & )>(_a, &MqttClient::environmentDataReceived, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (MqttClient::*)(const QString & )>(_a, &MqttClient::boatChanged, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (MqttClient::*)(const WaveConfig & )>(_a, &MqttClient::waveConfigChanged, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (MqttClient::*)(const WaveConfig & )>(_a, &MqttClient::waveConfigPublished, 10))
            return;
    }
}

const QMetaObject *MqttClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MqttClient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MqttClientE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int MqttClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void MqttClient::connectionStateChanged(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void MqttClient::messageReceived(const QString & _t1, const QByteArray & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void MqttClient::sensorDataReceived(const QString & _t1, const QJsonObject & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}

// SIGNAL 3
void MqttClient::errorOccurred(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void MqttClient::imuDataReceived(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void MqttClient::gpsDataReceived(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void MqttClient::speedDataReceived(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}

// SIGNAL 7
void MqttClient::environmentDataReceived(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1);
}

// SIGNAL 8
void MqttClient::boatChanged(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1);
}

// SIGNAL 9
void MqttClient::waveConfigChanged(const WaveConfig & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 9, nullptr, _t1);
}

// SIGNAL 10
void MqttClient::waveConfigPublished(const WaveConfig & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 10, nullptr, _t1);
}
QT_WARNING_POP
