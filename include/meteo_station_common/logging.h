#pragma once

#include <Arduino.h>
#include <config.h>

#if ENABLE_GRAFANA_LOGGING

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <vector>

#include <meteo_station_common/time_ntp.h>

namespace meteo {
namespace logging_detail {

inline constexpr size_t MAX_BATCH_LOGS = 20;

struct LogEntry {
    uint64_t timestamp;
    String message;
};

inline std::vector<LogEntry> logBuffer;

inline void sendBatch() {
    if (logBuffer.empty()) {
        return;
    }

    JsonDocument doc;
    JsonArray streams = doc["streams"].to<JsonArray>();
    JsonObject stream = streams.add<JsonObject>();

    JsonObject labels = stream["stream"].to<JsonObject>();
    labels["service_name"] = LOG_NAME;
    labels["fw"] = firmwareVersion;

    JsonArray values = stream["values"].to<JsonArray>();
    for (const auto& e : logBuffer) {
        JsonArray entry = values.add<JsonArray>();
        entry.add(String(e.timestamp));
        entry.add(e.message);
    }

    String body;
    serializeJson(doc, body);
    Serial.println(body);

    WiFiClientSecure tls;
    tls.setInsecure();

    HTTPClient http;
    http.begin(tls, GRAFANA_URL);
    http.setAuthorization(GRAFANA_USER, GRAFANA_TOKEN);
    http.addHeader("Content-Type", "application/json");

    int response = http.POST(body);
    if (response == 204) {
        Serial.printf("Logs sent: %i\n", response);
        logBuffer.clear();
    } else {
        Serial.printf("Cannot send logs: %i\n", response);
        String payload = http.getString();
        Serial.println(payload);
    }

    http.end();
}

inline void bufferLog(const String& level, const String& message) {
    (void)level;
    uint64_t ts = getTimestampNs();
    logBuffer.push_back({ ts, message });

    if (logBuffer.size() >= MAX_BATCH_LOGS) {
        sendBatch();
    }
}

} // namespace logging_detail
} // namespace meteo

inline void sendLog(const String& level, const String& msg) {
    Serial.println(msg);
    meteo::logging_detail::bufferLog(level, msg);
}

inline void flushLogsBeforeSleep() {
    meteo::logging_detail::sendBatch();
    delay(1000);
}

#else

inline void sendLog(const String& level, const String& msg) {
    (void)level;
    Serial.println(msg);
}

inline void flushLogsBeforeSleep() {
}

#endif
