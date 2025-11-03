#pragma once
#include <QDialog>
#include <QSlider>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QGroupBox>
#include <QDialogButtonBox>
#include "backend/waveconfig.h"

/**
 * @brief WaveConfig —— 波浪参数结构体
 */
// struct WaveConfig {
//     double amplitude;  // 波幅 [0,10] m
//     double period;     // 周期 [1,20] s
//     double direction;  // 方向 [0,6.28] rad
//     double steepness;  // 陡峭度 [0,1]
//     double gain;       // 增益 [0,2]
// };

/**
 * @brief WaveConfigDialog —— 可视化波浪参数设置对话框
 *
 * 功能特点：
 * 1. 每个参数有滑动条 + 数值输入框；
 * 2. 滑动条和输入框双向绑定；
 * 3. 点击“确定”发射 configChanged(config) 信号；
 * 4. 范围严格限定，防止超出；
 */
class WaveConfigDialog : public QDialog {
    Q_OBJECT
public:
    explicit WaveConfigDialog(QWidget* parent = nullptr);

    void setConfig(const WaveConfig& cfg);  // 载入现有配置
    WaveConfig getConfig() const;           // 获取当前配置

signals:
    void configChanged(const WaveConfig& config);  // 当点击“确定”时发射

private slots:
    void onApplyClicked();

private:
    // 内部创建单个参数行的方法
    QWidget* createParameterRow(const QString& name,
                                double min, double max,
                                double step,
                                const QString& unit,
                                QSlider*& slider,
                                QDoubleSpinBox*& spin);

private:
    QSlider *ampSlider_, *periodSlider_, *dirSlider_, *steepSlider_, *gainSlider_;
    QDoubleSpinBox *ampSpin_, *periodSpin_, *dirSpin_, *steepSpin_, *gainSpin_;
};
