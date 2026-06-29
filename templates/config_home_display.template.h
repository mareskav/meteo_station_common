#pragma once

#include <esp_adc_cal.h>
#include <models/weather_models.h>
#include <meteo_station_common/wifi.h>


inline constexpr char firmwareVersion[] PROGMEM =
#include "../version.txt"
;

// Device identity
#define DEVICE_NAME        "Meteostation display home"
#define LOG_NAME           "meteo-display-home"


// Wi-Fi
inline constexpr meteo::WifiCred WIFI_LIST[] = {
    { "ssid-1", "password-1" },
    { "ssid-2", "password-2" },
};
inline constexpr size_t WIFI_LIST_LEN = std::size(WIFI_LIST);


// Runtime
#define DEEP_SLEEP                  5              // minutes
#define TRY_TO_CONNECT_TIME         0.5            // minutes
#define UPDATE_COUNTS               7              // OTA check interval in wakeups
inline RTC_DATA_ATTR int updateCount = 0;


// ESP32, e-paper power, and sensors
#define ESP_BAUD                    115200
#define ESP_SDA_PIN                 21
#define ESP_SCL_PIN                 22
#define EPAPER_POWER_PIN            2
#define BATTERY_PIN                 34
#define USB_CHARGE_PIN              36
#define BATTERY_DEVIDER_RATIO       1.7693877551
#define SEALEVELPRESSURE_HPA        1013.25


// OTA
#define GITHUB_REPO_NAME                        "meteo_station_home_display"
#define GITHUB_MAIN_BRANCH                      "main"
inline constexpr char FW_VERSION_URL_GITHUB[] = "https://raw.githubusercontent.com/<owner>/" GITHUB_REPO_NAME "/" GITHUB_MAIN_BRANCH "/version.txt";
inline constexpr char FW_BIN_URL_GITHUB[]     = "https://raw.githubusercontent.com/<owner>/" GITHUB_REPO_NAME "/" GITHUB_MAIN_BRANCH "/firmware.bin";
inline constexpr char FW_COMMIT_URL[]         = "https://api.github.com/repos/<owner>/" GITHUB_REPO_NAME "/commits/" GITHUB_MAIN_BRANCH;
#define GITHUB_TOKEN                            "<github-token>"


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
#define TMEP_HOME_GET_URL    "https://tmep.cz/vystup-json.php?id=<home-id>&export_key=<home-export-key>&extended=1"
#define TMEP_OUT_GET_URL     "https://tmep.cz/vystup-json.php?id=<out-id>&export_key=<out-export-key>&extended=1"
#define TMEP_HOME_POST_URL   "https://example.tmep.cz/index.php?"


// Meteosource
#define METEOSOURCE_API_URL  "https://www.meteosource.com/api/v1/free/point?"
#define METEOSOURCE_API_KEY  "<meteosource-api-key>"
#define LAT                  "50.0000000"
#define LON                  "15.0000000"


// Logging
#define ENABLE_GRAFANA_LOGGING     0   // 0 = off, 1 = on
#define GRAFANA_URL                "https://logs.example.com/loki/api/v1/push"
#define GRAFANA_USER               "<grafana-user>"
#define GRAFANA_TOKEN              "<grafana-token>"


// E-paper pins and refresh policy
#define SS    5
#define DC    17
#define RST   16
#define BUSY  4
#define FULL_REFRESH_ONLY          true
#define FULL_REFRESH_INTERVAL      10
inline RTC_DATA_ATTR int refreshCount = FULL_REFRESH_INTERVAL;


// CO2 measurement
#define CO2_MEASURE_COUNTS         4
inline RTC_DATA_ATTR int co2Count = CO2_MEASURE_COUNTS;
inline RTC_DATA_ATTR float co2 = 0;
#define CO2_TEMP_DELTA_THRESHOLD   0.5f
inline RTC_DATA_ATTR float lastHomeTemp = -1000.0f;


// Weather prediction
#define PREDICTION_MEASURE_COUNTS  36
inline RTC_DATA_ATTR int predictionCount = PREDICTION_MEASURE_COUNTS;
inline RTC_DATA_ATTR WeatherPrediction weatherPrediction;


// Global values
inline esp_adc_cal_characteristics_t adc_chars;
inline HomeTMEPValues homeTMEPValues;
