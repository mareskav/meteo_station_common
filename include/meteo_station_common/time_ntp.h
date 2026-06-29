#pragma once

#include <Arduino.h>
#include <sys/time.h>
#include <time.h>

namespace meteo {
namespace time_detail {

inline constexpr const char* NTP1 = "pool.ntp.org";
inline constexpr const char* NTP2 = "time.google.com";
inline constexpr const char* NTP3 = "time.cloudflare.com";
inline constexpr const char* TZ_PRAGUE = "CET-1CEST,M3.5.0/2,M10.5.0/3";

inline uint64_t baseTimeNs = 0;
inline uint32_t baseMillis = 0;
inline bool timeSynced = false;

} // namespace time_detail
} // namespace meteo

inline void syncTime() {
    configTzTime(meteo::time_detail::TZ_PRAGUE,
                 meteo::time_detail::NTP1,
                 meteo::time_detail::NTP2,
                 meteo::time_detail::NTP3);

    struct tm local;
    if (!getLocalTime(&local, 15000)) {
        Serial.println("NTP sync FAILED");
        meteo::time_detail::timeSynced = false;
        return;
    }

    struct timeval tv;
    gettimeofday(&tv, nullptr);
    meteo::time_detail::baseTimeNs = uint64_t(tv.tv_sec) * 1000000000ULL
                                   + uint64_t(tv.tv_usec) * 1000ULL;
    meteo::time_detail::baseMillis = millis();
    meteo::time_detail::timeSynced = true;

    time_t now = time(nullptr);
    struct tm utc;
    gmtime_r(&now, &utc);

    Serial.printf("Time synced. LOCAL %02d:%02d (isdst=%d) | UTC %02d:%02d\n",
                  local.tm_hour, local.tm_min, local.tm_isdst,
                  utc.tm_hour, utc.tm_min);
}

inline void syncTimeWithTimeZone() {
    syncTime();
}

inline String getRtcDateTimeLocal() {
    struct tm t;
    if (!getLocalTime(&t)) {
        return "??.??. ??:??";
    }

    char buf[20];
    snprintf(buf, sizeof(buf), "%02d.%02d. %02d:%02d",
             t.tm_mday, t.tm_mon + 1, t.tm_hour, t.tm_min);
    return String(buf);
}

inline String getRtcDateTime() {
    return getRtcDateTimeLocal();
}

inline String getUtcDateTime() {
    time_t now = time(nullptr);
    struct tm utc;
    gmtime_r(&now, &utc);

    char buf[25];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02dT%02d:%02d:%02dZ",
             utc.tm_year + 1900, utc.tm_mon + 1, utc.tm_mday,
             utc.tm_hour, utc.tm_min, utc.tm_sec);
    return String(buf);
}

inline uint64_t getTimestampNs() {
    if (!meteo::time_detail::timeSynced) {
        return uint64_t(millis()) * 1000000ULL;
    }

    uint32_t nowMs = millis();
    uint32_t deltaMs = nowMs - meteo::time_detail::baseMillis;
    return meteo::time_detail::baseTimeNs + uint64_t(deltaMs) * 1000000ULL;
}
