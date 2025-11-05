/****************************************************************************
** Meta object code from reading C++ file 'main_window.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../include/main_window.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'main_window.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN10MainWindowE_t {};
} // unnamed namespace

template <> constexpr inline auto MainWindow::qt_create_metaobjectdata<qt_meta_tag_ZN10MainWindowE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "MainWindow",
        "sendWaveConfigToMqtt",
        "",
        "WaveConfig",
        "cfg",
        "sendBoatSelectionToMqtt",
        "boatName",
        "sendControlStatusToMqtt",
        "status",
        "boatChanged",
        "showVideoWindowRequested",
        "onBoatChanged",
        "name",
        "onOpenWaveConfig",
        "onWaveConfigChanged",
        "onWaveConfigPublished",
        "config",
        "onAutoClicked",
        "onManualClicked",
        "onStopClicked",
        "onShowVideoClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'sendWaveConfigToMqtt'
        QtMocHelpers::SignalData<void(const WaveConfig &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'sendBoatSelectionToMqtt'
        QtMocHelpers::SignalData<void(const QString &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 },
        }}),
        // Signal 'sendControlStatusToMqtt'
        QtMocHelpers::SignalData<void(const QString &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 },
        }}),
        // Signal 'boatChanged'
        QtMocHelpers::SignalData<void(const QString &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 },
        }}),
        // Signal 'showVideoWindowRequested'
        QtMocHelpers::SignalData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onBoatChanged'
        QtMocHelpers::SlotData<void(const QString &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 12 },
        }}),
        // Slot 'onOpenWaveConfig'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onWaveConfigChanged'
        QtMocHelpers::SlotData<void(const WaveConfig &)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Slot 'onWaveConfigPublished'
        QtMocHelpers::SlotData<void(const WaveConfig &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 16 },
        }}),
        // Slot 'onAutoClicked'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onManualClicked'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onStopClicked'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onShowVideoClicked'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MainWindow, qt_meta_tag_ZN10MainWindowE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10MainWindowE_t>.metaTypes,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MainWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->sendWaveConfigToMqtt((*reinterpret_cast< std::add_pointer_t<WaveConfig>>(_a[1]))); break;
        case 1: _t->sendBoatSelectionToMqtt((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->sendControlStatusToMqtt((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->boatChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->showVideoWindowRequested(); break;
        case 5: _t->onBoatChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->onOpenWaveConfig(); break;
        case 7: _t->onWaveConfigChanged((*reinterpret_cast< std::add_pointer_t<WaveConfig>>(_a[1]))); break;
        case 8: _t->onWaveConfigPublished((*reinterpret_cast< std::add_pointer_t<WaveConfig>>(_a[1]))); break;
        case 9: _t->onAutoClicked(); break;
        case 10: _t->onManualClicked(); break;
        case 11: _t->onStopClicked(); break;
        case 12: _t->onShowVideoClicked(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)(const WaveConfig & )>(_a, &MainWindow::sendWaveConfigToMqtt, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)(const QString & )>(_a, &MainWindow::sendBoatSelectionToMqtt, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)(const QString & )>(_a, &MainWindow::sendControlStatusToMqtt, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)(const QString & )>(_a, &MainWindow::boatChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (MainWindow::*)()>(_a, &MainWindow::showVideoWindowRequested, 4))
            return;
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.strings))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::sendWaveConfigToMqtt(const WaveConfig & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void MainWindow::sendBoatSelectionToMqtt(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void MainWindow::sendControlStatusToMqtt(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void MainWindow::boatChanged(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void MainWindow::showVideoWindowRequested()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP
