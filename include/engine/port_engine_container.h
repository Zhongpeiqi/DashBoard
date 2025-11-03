#pragma once
#include <QWidget>
#include "engine/engine_bar_widget.h"
#include "engine/rudder_gauge_widget.h"

/**
 * @brief PortEngineContainer —— 左舷引擎简化版
 * 顶部红条 PORT
 * 中间为 EngineBarWidget（垂直居中）
 * 底部为 RudderGaugeWidget
 */
class PortEngineContainer : public QWidget {
    Q_OBJECT
public:
    explicit PortEngineContainer(QWidget* parent = nullptr);
    void setData(double order, double rudder);

private:
    EngineBarWidget* bar_;
    RudderGaugeWidget* rudder_;
};
