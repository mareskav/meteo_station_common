#pragma once

#include <Arduino.h>
#include <WiFi.h>

namespace meteo {

struct WifiCred {
    const char* ssid;
    const char* pass;
};

inline bool tryConnectOnce(const char* ssid, const char* pass, uint32_t timeoutMs) {
    WiFi.setAutoReconnect(true);
    WiFi.persistent(false);
    WiFi.disconnect(true);
    delay(200);

    Serial.printf("\n[WiFi] Trying SSID: %s\n", ssid);
    WiFi.begin(ssid, pass);

    const uint32_t start = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - start) < timeoutMs) {
        delay(250);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        return true;
    }

    Serial.printf("\n[WiFi] Failed SSID: %s (status=%d)\n", ssid, static_cast<int>(WiFi.status()));
    return false;
}

inline bool connectToAnyWifi(const WifiCred* networks, size_t count, uint32_t perNetworkTimeoutMs) {
    for (size_t i = 0; i < count; i++) {
        if (tryConnectOnce(networks[i].ssid, networks[i].pass, perNetworkTimeoutMs)) {
            return true;
        }
    }

    return false;
}

} // namespace meteo
