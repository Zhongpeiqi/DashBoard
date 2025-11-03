#pragma once
#include <QWidget>
#include "engine/engine_bar_widget.h"
#include "engine/rudder_gauge_widget.h"

/**
 * @brief StbdEngineContainer —— 右舷引擎容器组件
 *
 * 功能说明：
 *  - 模仿左舷（Port）结构；
 *  - 顶部绿色条显示 "STBD"；
 *  - 中部显示 EngineBarWidget（引擎能量条）；
 *  - 底部显示 RudderGaugeWidget（舵角仪表）；
 *  - 用于展示右主机输出与舵角状态；
 */
class StbdEngineContainer : public QWidget {
    Q_OBJECT
public:
    explicit StbdEngineContainer(QWidget* parent = nullptr);

    /**
     * @brief setData 更新右舷引擎数据
     * @param order 引擎输出（推力百分比或命令值）
     * @param rudder 舵角角度（单位°）
     */
    void setData(double order, double rudder);

private:
    EngineBarWidget* bar_;       ///< 引擎能量条控件
    RudderGaugeWidget* rudder_;  ///< 舵角仪表控件
};
