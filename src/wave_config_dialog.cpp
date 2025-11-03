#include "wave_config_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStyle>
#include <QDebug>

/**
 * @brief WaveConfigDialog 构造函数
 */
WaveConfigDialog::WaveConfigDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("风浪参数设置");
    resize(500, 400);
    setModal(true);  // 模态窗口，阻塞主窗口操作

    // === 创建主布局 ===
    auto* mainLayout = new QVBoxLayout(this);

    /**
     * 依次创建波浪各项参数设置行
     * 每一行由：标签 + 滑动条 + 数值输入框 + 单位组成
     */

    // 波幅 (0-10 m) —— 支持两位小数
    auto* ampRow = createParameterRow("Amplitude", 0.0, 10.0, 0.01, "m", ampSlider_, ampSpin_);
    ampSpin_->setDecimals(2); //  强制显示两位小数

    // 周期 (1-20 s)
    auto* periodRow = createParameterRow("Period", 1.0, 20.0, 0.1, "s", periodSlider_, periodSpin_);

    // 方向 (0-6.28 rad)
    auto* dirRow = createParameterRow("Direction", 0.0, 6.28, 0.01, "rad", dirSlider_, dirSpin_);

    // 陡峭度 (0-1)
    auto* steepRow = createParameterRow("Steepness", 0.0, 1.0, 0.01, "", steepSlider_, steepSpin_);

    // 增益 (0-2) —— 去掉单位符号“×”
    auto* gainRow = createParameterRow("Gain", 0.0, 2.0, 0.01, "", gainSlider_, gainSpin_);

    // 将所有参数行加入主布局
    mainLayout->addWidget(ampRow);
    mainLayout->addWidget(periodRow);
    mainLayout->addWidget(dirRow);
    mainLayout->addWidget(steepRow);
    mainLayout->addWidget(gainRow);

    // === 底部按钮组（确定 / 取消） ===
    auto* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &WaveConfigDialog::onApplyClicked);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &WaveConfigDialog::reject);
    mainLayout->addWidget(buttonBox);

    // === 统一界面样式 ===
    setStyleSheet(R"(
        QLabel { color: white; font-size: 14px; }
        QDialog { background-color: #222; }
        QSlider::groove:horizontal { height: 6px; background: #444; border-radius: 3px; }
        QSlider::handle:horizontal { background: #00c896; width: 14px; border-radius: 7px; margin: -4px 0; }
    )");
}

/**
 * @brief 创建参数行：滑动条 + 数值输入框 + 单位标签
 */
QWidget* WaveConfigDialog::createParameterRow(const QString& name,
                                              double min, double max,
                                              double step,
                                              const QString& unit,
                                              QSlider*& slider,
                                              QDoubleSpinBox*& spin)
{
    QWidget* row = new QWidget(this);
    auto* layout = new QHBoxLayout(row);
    layout->setContentsMargins(8, 2, 8, 2);  // 内边距
    layout->setSpacing(10);                  // 控件间距

    QLabel* label = new QLabel(name + ":", row);
    label->setFixedWidth(90);  // 参数名称列固定宽度

    // ===============================
    // 滑动条设置
    // ===============================
    slider = new QSlider(Qt::Horizontal, row);
    int precision = static_cast<int>(1.0 / step);  // step=0.01 → 精度100
    slider->setRange(static_cast<int>(min * precision),
                     static_cast<int>(max * precision));

    // ===============================
    // 数值输入框设置
    // ===============================
    spin = new QDoubleSpinBox(row);
    spin->setRange(min, max);
    spin->setSingleStep(step);
    spin->setDecimals(step <= 0.01 ? 2 : (step <= 0.1 ? 1 : 0));  // 根据步长设置小数位数
    spin->setFixedWidth(80);

    // ===============================
    // 单位标签
    // ===============================
    QLabel* unitLabel = new QLabel(unit, row);
    unitLabel->setFixedWidth(40);

    // 添加到布局中
    layout->addWidget(label);
    layout->addWidget(slider, 1);  // 滑动条自适应伸展
    layout->addWidget(spin);
    layout->addWidget(unitLabel);

    // ===============================
    // 双向绑定逻辑
    // ===============================

    // 滑动条变化 -> 更新 spinbox
    connect(slider, &QSlider::valueChanged, [=](int val) {
        double scaled = static_cast<double>(val) / precision;
        spin->blockSignals(true);
        spin->setValue(scaled);
        spin->blockSignals(false);
    });

    // spinbox变化 -> 更新滑动条
    connect(spin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double val) {
        slider->blockSignals(true);
        slider->setValue(static_cast<int>(val * precision));
        slider->blockSignals(false);
    });

    // 初始化一致
    slider->setValue(static_cast<int>(min * precision));
    spin->setValue(min);

    return row;
}

/**
 * @brief 从对话框获取当前配置
 */
WaveConfig WaveConfigDialog::getConfig() const {
    return {
        ampSpin_->value(),
        periodSpin_->value(),
        dirSpin_->value(),
        steepSpin_->value(),
        gainSpin_->value()
    };
}

/**
 * @brief 设置初始配置（例如加载当前系统状态）
 */
void WaveConfigDialog::setConfig(const WaveConfig& cfg) {
    ampSpin_->setValue(cfg.amplitude);
    periodSpin_->setValue(cfg.period);
    dirSpin_->setValue(cfg.direction);
    steepSpin_->setValue(cfg.steepness);
    gainSpin_->setValue(cfg.gain);
}

/**
 * @brief 点击“确定”按钮时发射信号并关闭窗口
 */
void WaveConfigDialog::onApplyClicked() {
    qDebug() << "波浪配置对话框：点击确定按钮";

        // 获取当前配置
        WaveConfig config = getConfig();
        qDebug() << "波浪配置参数:"
                 << "幅值:" << config.amplitude
                 << "周期:" << config.period
                 << "方向:" << config.direction
                 << "陡峭:" << config.steepness
                 << "增益:" << config.gain;

        // 发射配置变化信号
        qDebug() << "发射 configChanged 信号";
        emit configChanged(config);

        // 关闭对话框
        qDebug() << "关闭波浪配置对话框";
        accept();
}
