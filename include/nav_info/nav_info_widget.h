#ifndef NAV_INFO_WIDGET_H
#define NAV_INFO_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

// ======================
// 前向声明 Pitch 仪表组件
// ======================
class PitchGaugeWidget;

/**
 * @brief NavInfoWidget —— 航海信息显示控件
 *
 * 显示以下导航参数：
 *   - ALT（高度）
 *   - LAT / LON（经纬度）
 *   - COG（航向）
 *   - SOG（航速）
 *   - 最下方为 PitchGaugeWidget（俯仰仪表）
 *
 * 特点：
 *   - 黑底白字风格；
 *   - 自动伸缩布局；
 *   - 经纬度根据正负显示 N/S、E/W；
 *   - 俯仰仪表底部独立显示（内部自带标题与角度）。
 */
class NavInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NavInfoWidget(QWidget *parent = nullptr);

    // ===== 外部数据更新接口 =====
    void setAlt(double alt);
    void setLon(double lon);
    void setLat(double lat);
    void setCog(double cog);
    void setSog(double sog);
    void setPitch(double pitch);  // 同步更新俯仰仪表角度

private:
    // ==== UI元素声明 ====
    QLabel *altLabel; QLabel *altValue;   // 高度
    QLabel *lonLabel; QLabel *lonValue;   // 经度
    QLabel *latLabel; QLabel *latValue;   // 纬度
    QLabel *cogLabel; QLabel *cogValue;   // 航向
    QLabel *sogLabel; QLabel *sogValue;   // 航速

    PitchGaugeWidget *pitchGauge;         // 保留俯仰仪表

    // ==== 内部函数 ====
    void setupLayout();                    // 初始化布局
};

#endif // NAV_INFO_WIDGET_H
