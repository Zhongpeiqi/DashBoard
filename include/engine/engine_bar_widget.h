#pragma once
#include <QWidget>
#include <QTimer>

class EngineBarWidget : public QWidget {
    Q_OBJECT
public:
    explicit EngineBarWidget(QWidget* parent = nullptr);
    void setValue(double v);   // 设置目标值（外部调用）

protected:
    void paintEvent(QPaintEvent*) override;

private:
    double value_ = 0.0;        // 目标值（真实输入）
    double displayValue_ = 0.0; // 用于平滑显示的值
};
