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

} // namespace meteo
