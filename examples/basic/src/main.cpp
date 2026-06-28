#include <Arduino.h>
#include <esp_sleep.h>

#include <meteo_station_common/battery.h>
#include <meteo_station_common/email_helpers.h>
#include <meteo_station_common/sleep.h>
#include <meteo_station_common/wifi.h>

constexpr meteo::WifiCred WIFI_LIST[] = {
    { "example-ssid-1", "example-pass-1" },
    { "example-ssid-2", "example-pass-2" },
};

constexpr uint8_t BATTERY_PIN = 0;
constexpr double BATTERY_DIVIDER_RATIO = 1.7693877551;

void setup() {
    Serial.begin(115200);

    meteo::setupBatteryPin(BATTERY_PIN);

    const double batteryVoltage = meteo::readBatteryVoltage(BATTERY_PIN, BATTERY_DIVIDER_RATIO);
    const double batteryPct = meteo::batteryPercentage(batteryVoltage);
    const int sleepMinutes = meteo::sleepMinutesForBattery(batteryPct);
    const uint64_t sleepUs = meteo::minutesToWakeupUs(sleepMinutes);

    const String bullets = meteo::commitToHtmlBullets("Improve OTA\nFix sleep calculation");
    const String escaped = meteo::htmlEscape("<meteo>");

    Serial.printf("Battery: %.2f V, %.1f %%\n", batteryVoltage, batteryPct);
    Serial.printf("Sleep: %d min / %llu us\n", sleepMinutes, sleepUs);
    Serial.println(bullets);
    Serial.println(escaped);

    (void)WIFI_LIST;
}

void loop() {
}
