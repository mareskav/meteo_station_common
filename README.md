# meteo-station-common

Shared header-only helpers for ESP32 PlatformIO meteo station projects.

The library contains only reusable code. It intentionally does not contain
Wi-Fi credentials, SMTP passwords, GitHub tokens, Grafana tokens, API keys, TMEP
URLs, export keys, device names, pin numbers, or project-specific configuration.
Keep those values in each station project's private `config.h` or in a private
file included by `config.h`.

## What Is Included

- `battery.h`: battery voltage and percentage helpers.
- `sleep.h`: battery-based sleep interval helpers and safe minute-to-microsecond conversion.
- `wifi.h`: connect to one of multiple configured Wi-Fi networks.
- `email_helpers.h`: HTML escaping and commit-message-to-bullet-list formatting.
- `templates/`: public-safe `config.h` templates for station projects.

All public symbols live in the `meteo` namespace.

## Installation

After publishing this repository on GitHub, add it to each station project's
`platformio.ini`:

```ini
lib_deps =
    https://github.com/mareskav/meteo_station_common.git
```

For local testing before publishing, use a relative or absolute path:

```ini
lib_deps =
    ../meteo_station_common
```

Then include only the helpers needed by the project:

```cpp
#include <meteo_station_common/battery.h>
#include <meteo_station_common/email_helpers.h>
#include <meteo_station_common/sleep.h>
#include <meteo_station_common/wifi.h>
```

## Example

```cpp
#include <Arduino.h>
#include <WiFi.h>
#include <meteo_station_common/battery.h>
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

    const bool connected = meteo::connectToAnyWifi(
        WIFI_LIST,
        sizeof(WIFI_LIST) / sizeof(WIFI_LIST[0]),
        30000
    );

    meteo::setupBatteryPin(BATTERY_PIN);
    const double voltage = meteo::readBatteryVoltage(BATTERY_PIN, BATTERY_DIVIDER_RATIO);
    const double pct = meteo::batteryPercentage(voltage);
    const int sleepMinutes = meteo::sleepMinutesForBattery(pct);

    Serial.printf("WiFi: %s, battery: %.2f V, %.1f %%, sleep: %d min\n",
                  connected ? "connected" : "failed",
                  voltage,
                  pct,
                  sleepMinutes);
}

void loop() {
}
```

## API Notes

### Battery

`meteo::setupBatteryPin(pin)` configures the ADC attenuation for a battery
measurement pin.

`meteo::readBatteryVoltage(pin, dividerRatio)` reads millivolts through the
Arduino ESP32 core and applies the voltage divider ratio.

`meteo::batteryPercentage(voltage)` maps a 3.0-4.2 V Li-Ion range to 0-100 % and
clamps values outside the range.

### Sleep

`meteo::sleepMinutesForBattery(percent)` returns the current dynamic sleep
policy:

| Battery | Sleep |
| --- | ---: |
| <= 30 % | 720 min |
| <= 40 % | 120 min |
| <= 50 % | 60 min |
| <= 60 % | 30 min |
| <= 70 % | 20 min |
| <= 80 % | 15 min |
| <= 90 % | 10 min |
| > 90 % | 5 min |

`meteo::minutesToWakeupUs(minutes)` converts minutes to microseconds using
64-bit arithmetic. Use it with `esp_sleep_enable_timer_wakeup()` to avoid
overflow on long sleep intervals.

### Wi-Fi

`meteo::connectToAnyWifi(networks, count, timeoutMs)` tries configured networks
in order and returns `true` after the first successful connection.

This helper does not call deep sleep on failure. The application should decide
whether to sleep, retry, or continue offline.

### Email Helpers

`meteo::htmlEscape(text)` escapes text for simple HTML email bodies.

`meteo::commitToHtmlBullets(commitMessage)` splits a commit message by newlines
and commas and returns `<li>...</li>` items.

## Migrating The Station Repos

The existing station projects currently keep local helper copies so they remain
buildable without an external dependency. A safe migration path is:

1. Publish this repository as public GitHub repo.
2. Add it to one station project's `platformio.ini` via `lib_deps`.
3. Replace one local helper include at a time with the `meteo_station_common`
   include.
4. Prefix calls with `meteo::`.
5. Run `pio run`.
6. Repeat for the remaining station projects.

Keep each station's `config.h` project-specific. Device names, pins, endpoints,
tokens, credentials, and per-station thresholds belong there, not in this shared
library.

## Config Templates

The `templates/` folder contains public-safe config templates:

- `config_home.template.h`: indoor station without display.
- `config_home_display.template.h`: indoor station with e-paper display.
- `config_out.template.h`: outdoor BME280 station.

Use them as a schema for private project configs:

```powershell
Copy-Item templates\config_home.template.h ..\meteo_station_home\src\config.template.h
```

Then keep the real `src/config.h` private in each project. The template should
show all required macros, globals, and sections, but every credential, token,
API key, URL, location, and device-specific value must stay as a placeholder.

## Public Repo Checklist

Before pushing this library to a public GitHub repository:

- Run a secret scan against the repo.
- Do not add firmware binaries.
- Do not add `.pio`, `.vscode`, `.idea`, or generated build files.
- Keep examples using placeholder credentials only.
- Keep station-specific configs in the private station repositories.
