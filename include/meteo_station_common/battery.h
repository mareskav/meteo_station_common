#pragma once

#include <Arduino.h>

namespace meteo {

inline double batteryPercentage(double voltage) {
    const double pct = (voltage - 3.0) / (4.2 - 3.0) * 100.0;
    if (pct < 0.0) return 0.0;
    if (pct > 100.0) return 100.0;
    return pct;
}

inline double readBatteryVoltage(uint8_t pin, double dividerRatio) {
    return (analogReadMilliVolts(pin) / 1000.0) * dividerRatio;
}

inline void setupBatteryPin(uint8_t pin) {
    analogSetPinAttenuation(pin, ADC_11db);
}

inline int co2MeasureCountsForBattery(double batteryPct,
                                      int baseCounts,
                                      int sleepMinutes,
                                      int maxIntervalMinutes = 120) {
    if (baseCounts < 1) {
        baseCounts = 1;
    }
    if (sleepMinutes < 1) {
        sleepMinutes = 1;
    }

    int intervalMinutes = baseCounts * sleepMinutes;
    if (batteryPct < 60.0) {
        int lowBatterySteps = 0;
        for (double threshold = 60.0; threshold > 0.0 && batteryPct < threshold; threshold -= 5.0) {
            ++lowBatterySteps;
        }
        intervalMinutes += lowBatterySteps * 10;
    }

    if (intervalMinutes > maxIntervalMinutes) {
        intervalMinutes = maxIntervalMinutes;
    }

    return (intervalMinutes + sleepMinutes - 1) / sleepMinutes;
}

} // namespace meteo
