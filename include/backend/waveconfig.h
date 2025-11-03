#ifndef WAVECONFIG_H
#define WAVECONFIG_H

#include <QJsonObject>

/**
 * @brief WaveConfig —— 波浪参数结构体
 */
struct WaveConfig {
    double amplitude;  // 波幅 [0,10] m
    double period;     // 周期 [1,20] s
    double direction;  // 方向 [0,6.28] rad
    double steepness;  // 陡峭度 [0,1]
    double gain;       // 增益 [0,2]

    // 转换为JSON对象
    QJsonObject toJson() const {
        QJsonObject json;
        json["amplitude"] = amplitude;
        json["period"] = period;
        json["direction"] = direction;
        json["steepness"] = steepness;
        json["gain"] = gain;
        return json;
    }

    // 从JSON对象解析
    void fromJson(const QJsonObject& json) {
        amplitude = json.value("amplitude").toDouble();
        period = json.value("period").toDouble();
        direction = json.value("direction").toDouble();
        steepness = json.value("steepness").toDouble();
        gain = json.value("gain").toDouble();
    }
};

#endif // WAVECONFIG_H
