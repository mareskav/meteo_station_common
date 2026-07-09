#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <SparkFun_SCD4x_Arduino_Library.h>

#include <meteo_station_common/logging.h>

namespace meteo {

inline SCD4x scd41;

inline constexpr uint32_t SCD41_CO2_MEASUREMENT_TIMEOUT_MS = 45000;
inline constexpr uint32_t SCD41_SINGLE_SHOT_TIMEOUT_MS = 7000;
inline constexpr uint16_t SCD41_I2C_ADDRESS = 0x62;
inline constexpr uint16_t SCD41_POWER_DOWN_COMMAND = 0x36E0;
inline constexpr uint16_t SCD41_WAKE_UP_COMMAND = 0x36F6;
inline constexpr uint16_t SCD41_WAKE_UP_DELAY_MS = 30;

inline void sendRawScd41Command(TwoWire& wire, uint16_t command) {
    wire.beginTransmission(SCD41_I2C_ADDRESS);
    wire.write(command >> 8);
    wire.write(command & 0xFF);
    wire.endTransmission();
}

inline void wakeUpScd41(TwoWire& wire = Wire,
                        uint16_t wakeDelayMs = SCD41_WAKE_UP_DELAY_MS) {
    sendRawScd41Command(wire, SCD41_WAKE_UP_COMMAND);
    delay(wakeDelayMs);
}

inline void powerDownScd41(SCD4x& sensor = scd41, bool stopPeriodicMeasurement = true) {
    if (stopPeriodicMeasurement) {
        sensor.stopPeriodicMeasurement();
        delay(500);
    }
    sensor.sendCommand(SCD41_POWER_DOWN_COMMAND);
}

inline bool setupScd41LowPower(uint8_t sdaPin,
                               uint8_t sclPin,
                               SCD4x& sensor = scd41,
                               TwoWire& wire = Wire,
                               uint8_t maxAttempts = 5,
                               bool autoCalibrate = true) {
    sendLog("info", "SCD41 setup");

    wire.begin(sdaPin, sclPin);
    wakeUpScd41(wire);
    // Give the sensor a short settle time after power-up or wake-up.
    delay(150);

    for (uint8_t attempt = 1; attempt <= maxAttempts; attempt++) {
        if (sensor.begin(wire, false, autoCalibrate)) {
            if (sensor.startLowPowerPeriodicMeasurement()) {
                sendLog("info", "SCD41 low power mode enabled.");
                return true;
            }

            sendLog("error", "Failed to start SCD41 low power measurement.");
            powerDownScd41(sensor);
            return false;
        }

        sendLog("error", "SCD41 begin failed, retry " + String(attempt));
        wire.end();
        delay(50);
        wire.begin(sdaPin, sclPin);
        wakeUpScd41(wire);
        delay(150);
    }

    sendLog("error", "SCD41 was not set correctly after retries.");
    return false;
}

inline bool setupScd41SingleShot(uint8_t sdaPin,
                                 uint8_t sclPin,
                                 SCD4x& sensor = scd41,
                                 TwoWire& wire = Wire,
                                 uint8_t maxAttempts = 5,
                                 bool autoCalibrate = true) {
    sendLog("info", "SCD41 setup");

    wire.begin(sdaPin, sclPin);
    wakeUpScd41(wire);
    // Give the sensor a short settle time after power-up or wake-up.
    delay(150);

    for (uint8_t attempt = 1; attempt <= maxAttempts; attempt++) {
        if (sensor.begin(wire, false, autoCalibrate)) {
            sendLog("info", "SCD41 single-shot mode ready.");
            return true;
        }

        sendLog("error", "SCD41 begin failed, retry " + String(attempt));
        wire.end();
        delay(50);
        wire.begin(sdaPin, sclPin);
        wakeUpScd41(wire);
        delay(150);
    }

    sendLog("error", "SCD41 was not set correctly after retries.");
    return false;
}

inline bool measureScd41CO2(float& co2Ppm,
                            SCD4x& sensor = scd41,
                            uint32_t timeoutMs = SCD41_CO2_MEASUREMENT_TIMEOUT_MS,
                            uint16_t pollDelayMs = 250) {
    sendLog("info", "Waiting for CO2 measurement.");

    // In low-power periodic mode, the first fresh measurement takes about 30 s.
    const uint32_t start = millis();
    while (!sensor.readMeasurement()) {
        if ((millis() - start) >= timeoutMs) {
            sendLog("error", "CO2 measurement timed out.");
            powerDownScd41(sensor);
            return false;
        }

        Serial.print(".");
        delay(pollDelayMs);
    }

    co2Ppm = sensor.getCO2();
    sendLog("info", "CO2: " + String(co2Ppm));

    powerDownScd41(sensor);
    return true;
}

inline bool measureScd41SingleShotCO2(float& co2Ppm,
                                      SCD4x& sensor = scd41,
                                      uint32_t timeoutMs = SCD41_SINGLE_SHOT_TIMEOUT_MS,
                                      uint16_t pollDelayMs = 250) {
    sendLog("info", "Starting CO2 single-shot measurement.");

    if (!sensor.measureSingleShot()) {
        sendLog("error", "Failed to start SCD41 single-shot measurement.");
        powerDownScd41(sensor, false);
        return false;
    }

    const uint32_t start = millis();
    while (!sensor.readMeasurement()) {
        if ((millis() - start) >= timeoutMs) {
            sendLog("error", "CO2 single-shot measurement timed out.");
            powerDownScd41(sensor, false);
            return false;
        }

        Serial.print(".");
        delay(pollDelayMs);
    }

    co2Ppm = sensor.getCO2();
    sendLog("info", "CO2: " + String(co2Ppm));

    powerDownScd41(sensor, false);
    return true;
}

} // namespace meteo
