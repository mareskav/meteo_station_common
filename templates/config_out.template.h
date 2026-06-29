#pragma once

#include <models/weather_models.h>
#include <meteo_station_common/wifi.h>


inline constexpr char firmwareVersion[] PROGMEM =
#include "../version.txt"
;

// Device identity
#define DEVICE_NAME        "Meteostation outdoor"
#define LOG_NAME           "meteo-out"


// Wi-Fi
inline constexpr meteo::WifiCred WIFI_LIST[] = {
    { "ssid-1", "password-1" },
    { "ssid-2", "password-2" },
};
inline constexpr size_t WIFI_LIST_LEN = std::size(WIFI_LIST);


// Runtime
#define TRY_TO_CONNECT_TIME         0.5            // minutes
#define UPDATE_COUNTS               7              // OTA check interval in wakeups
inline RTC_DATA_ATTR int updateCount = 0;


// ESP32 and sensors
#define ESP_BAUD                    115200
#define ESP_SDA_PIN                 19
#define ESP_SCL_PIN                 18
#define BATTERY_PIN                 0
#define BATTERY_DEVIDER_RATIO       1.7693877551
#define SEALEVELPRESSURE_HPA        1013.25


// OTA
#define GITHUB_REPO_NAME                          "meteo_station_out"
#define GITHUB_MAIN_BRANCH                        "main"
inline constexpr char FW_VERSION_URL_GITHUB[]   = "https://raw.githubusercontent.com/<owner>/" GITHUB_REPO_NAME "/" GITHUB_MAIN_BRANCH "/version.txt";
inline constexpr char FW_BIN_URL_GITHUB[]       = "https://raw.githubusercontent.com/<owner>/" GITHUB_REPO_NAME "/" GITHUB_MAIN_BRANCH "/firmware.bin";
inline constexpr char FW_COMMIT_URL[]           = "https://api.github.com/repos/<owner>/" GITHUB_REPO_NAME "/commits/" GITHUB_MAIN_BRANCH;
#define GITHUB_TOKEN                              "<github-token>"


// OTA fallback
#define CLOUDFLARE_R2_HASH                             "<cloudflare-r2-public-hash>"
inline constexpr char FW_VERSION_URL_CLOUDFLARE_R2[] = "https://pub-" CLOUDFLARE_R2_HASH ".r2.dev/" GITHUB_REPO_NAME "/version.txt";
inline constexpr char FW_BIN_URL_CLOUDFLARE_R2[]     = "https://pub-" CLOUDFLARE_R2_HASH ".r2.dev/" GITHUB_REPO_NAME "/firmware.bin";


// Email
#define SMTP_HOST                "smtp.example.com"
#define SMTP_PORT                465
#define SENDER_EMAIL             "sender@example.com"
#define SENDER_PASSWORD          "<smtp-password>"
#define RECIPIENT_NAME           "Recipient"
#define RECIPIENT_EMAIL          "recipient@example.com"
#define ENABLE_SECOND_RECIPIENT  0
#define RECIPIENT_NAME_2         ""
#define RECIPIENT_EMAIL_2        ""


// TMEP
#define TMEP_OUT_POST_URL   "https://example.tmep.cz/index.php?"


// Logging
#define ENABLE_GRAFANA_LOGGING     0
#define GRAFANA_URL                "https://logs.example.com/loki/api/v1/push"
#define GRAFANA_USER               "<grafana-user>"
#define GRAFANA_TOKEN              "<grafana-token>"


// Global values
inline OutTMEPValues TMEPValues;
