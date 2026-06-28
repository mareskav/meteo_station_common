#pragma once

#include <Arduino.h>

namespace meteo {

inline int sleepMinutesForBattery(double batteryPercentage) {
    if (batteryPercentage <= 30.0) return 720;
    if (batteryPercentage <= 40.0) return 120;
    if (batteryPercentage <= 50.0) return 60;
    if (batteryPercentage <= 60.0) return 30;
    if (batteryPercentage <= 70.0) return 20;
    if (batteryPercentage <= 80.0) return 15;
    if (batteryPercentage <= 90.0) return 10;
    return 5;
}

inline uint64_t minutesToWakeupUs(uint32_t minutes) {
    return uint64_t(minutes) * 60ULL * 1000000ULL;
}

} // namespace meteo
