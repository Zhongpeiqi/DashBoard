#include "nav_info/nav_info_widget.h"
#include "nav_info/pitch_gauge_widget.h"
#include <QFont>
#include <QtMath>

// ============================================================
// 构造函数 —— 设置黑色背景 + 初始化布局
// ============================================================
NavInfoWidget::NavInfoWidget(QWidget *parent)
    : QWidget(parent)
{
    // 统一黑色背景风格
    this->setStyleSheet("background-color: black;");  // 控制整体背景

    setupLayout();  // 构建界面布局
}

// ============================================================
// 初始化主布局结构（样式与对齐方式在此直接设置）
// ============================================================
void NavInfoWidget::setupLayout()
{
    // ------------------ 公共字体 ------------------
    QFont labelFont("Arial", 20, QFont::Bold);   // 标签字体（ALT、COG等）
    QFont valueFont("Arial", 25, QFont::Bold);   // 数值字体（13.4 m、8.5 Kn）

    // ============================================================
    // 1️⃣ ALT 区域（左对齐）
    // ============================================================
    altLabel = new QLabel("ALT");
    altValue = new QLabel("-- m");

    altLabel->setFont(labelFont);
    altValue->setFont(valueFont);
    altLabel->setStyleSheet("color: #9E9E9E;");
    altValue->setStyleSheet("color: white;");
    altLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    altValue->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    QVBoxLayout *altLayout = new QVBoxLayout();
    altLayout->addWidget(altLabel);
    altLayout->addWidget(altValue);
    altLayout->setSpacing(8);

    // ============================================================
    // 2️⃣ LAT / LON 区域
    // ============================================================
    latLabel = new QLabel("LAT");
    latValue = new QLabel("--°N");
    lonLabel = new QLabel("LON");
    lonValue = new QLabel("--°E");

    latLabel->setFont(labelFont);
    latValue->setFont(valueFont);
    lonLabel->setFont(labelFont);
    lonValue->setFont(valueFont);

    latLabel->setStyleSheet("color: #9E9E9E;");
    latValue->setStyleSheet("color: white;");
    lonLabel->setStyleSheet("color: #9E9E9E;");
    lonValue->setStyleSheet("color: white;");

    // LON 左对齐，LAT 右对齐
    lonLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    lonValue->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    latLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    latValue->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QVBoxLayout *lonLayout = new QVBoxLayout();
    lonLayout->addWidget(lonLabel);
    lonLayout->addWidget(lonValue);
    lonLayout->setSpacing(8);

    QVBoxLayout *latLayout = new QVBoxLayout();
    latLayout->addWidget(latLabel);
    latLayout->addWidget(latValue);
    latLayout->setSpacing(8);

    QHBoxLayout *latLonRow = new QHBoxLayout();
    latLonRow->addLayout(lonLayout);
    latLonRow->addStretch(2);
    latLonRow->addLayout(latLayout);

    // ============================================================
    // 3️⃣ COG / SOG 区域
    // ============================================================
    cogLabel = new QLabel("YAW");
    cogValue = new QLabel("--°");
    sogLabel = new QLabel("SOG");
    sogValue = new QLabel("-- Kn");

    cogLabel->setFont(labelFont);
    cogValue->setFont(valueFont);
    sogLabel->setFont(labelFont);
    sogValue->setFont(valueFont);

    cogLabel->setStyleSheet("color: #9E9E9E;");
    cogValue->setStyleSheet("color: white;");
    sogLabel->setStyleSheet("color: #9E9E9E;");
    sogValue->setStyleSheet("color: white;");

    // 左右对齐
    cogLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    cogValue->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    sogLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    sogValue->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QVBoxLayout *cogLayout = new QVBoxLayout();
    cogLayout->addWidget(cogLabel);
    cogLayout->addWidget(cogValue);
    cogLayout->setSpacing(8);

    QVBoxLayout *sogLayout = new QVBoxLayout();
    sogLayout->addWidget(sogLabel);
    sogLayout->addWidget(sogValue);
    sogLayout->setSpacing(8);

    QHBoxLayout *cogSogRow = new QHBoxLayout();
    cogSogRow->addLayout(cogLayout);
    cogSogRow->addStretch(2);
    cogSogRow->addLayout(sogLayout);

    // ============================================================
    //PITCH 仪表区
    // ============================================================
    pitchGauge = new PitchGaugeWidget(this);
    pitchGauge->setMinimumHeight(240);
    pitchGauge->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    // ============================================================
    //主布局：垂直堆叠
    // ============================================================
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(altLayout);   // index 0
    mainLayout->addLayout(latLonRow);   // index 1
    mainLayout->addLayout(cogSogRow);   // index 2
    mainLayout->addWidget(pitchGauge);  // index 3

    // === 伸缩比例（2:2:2:4）===
    mainLayout->setStretch(0, 2);
    mainLayout->setStretch(1, 2);
    mainLayout->setStretch(2, 2);
    mainLayout->setStretch(3, 4);

    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(12);
    setLayout(mainLayout);
}

// ============================================================
// 数据更新函数（实时刷新 UI）
// ============================================================
void NavInfoWidget::setAlt(double alt)
{
    // 显示高度（保留 1 位小数）
    altValue->setText(QString::number(alt, 'f', 1) + " m");
}

void NavInfoWidget::setLon(double lon)
{
    // ------------------------------------------------------------
    // 经度：正数为东经 E，负数为西经 W
    // ------------------------------------------------------------
    QString dir = (lon >= 0.0) ? "E" : "W";
    double absLon = fabs(lon);

    // 超出范围修正（确保在 0~180° 内）
    absLon = fmod(absLon, 360.0);
    if (absLon > 180.0)
        absLon = 360.0 - absLon;

    // 保留 1位小数更精准
    QString text = QString::number(absLon, 'f', 1) + "°" + dir;
    lonValue->setText(text);

}
void NavInfoWidget::setLat(double lat)
{
    // ------------------------------------------------------------
    // 纬度：正数为北纬 N，负数为南纬 S
    // ------------------------------------------------------------
    QString dir = (lat >= 0.0) ? "N" : "S";
    double absLat = fabs(lat);

    // 超出范围修正（确保在 0~90° 内）
    absLat = fmod(absLat, 180.0);
    if (absLat > 90.0)
        absLat = 180.0 - absLat;

    QString text = QString::number(absLat, 'f', 1) + "°" + dir;
    latValue->setText(text);

}


void NavInfoWidget::setCog(double cog)
{
    // 航向角范围 0~360
    double normCog = fmod(cog, 360.0);
    if (normCog < 0) normCog += 360.0;
    cogValue->setText(QString::number(normCog, 'f', 1) + "°");
}

void NavInfoWidget::setSog(double sog)
{
    // 航速（单位 Kn，保留 1 位小数）
    if (sog < 0) sog = 0;
    sogValue->setText(QString::number(sog, 'f', 1) + " Kn");
}

void NavInfoWidget::setPitch(double pitch)
{
    if (pitchGauge)
        pitchGauge->setPitch(pitch);
}
