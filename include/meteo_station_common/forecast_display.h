#pragma once

// Five-day forecast renderer for the 400x300 e-paper layout shared by the
// display projects. It has no dependency on a global `display` or a project
// config.h: the caller passes the GxEPD2/Adafruit_GFX display and an icon lookup.
//
// `Prediction` must expose `fiveDays[5]`, each with `day`, `maxTemp`, `minTemp`,
// `rainProbability` (C strings; empty = not available) and `iconCode` (int).
// `iconFor(int iconCode)` returns the 40x40 bitmap to draw
// (see weather_icons.h: forecastCodeToIcon).

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>

namespace meteo_station_common {

inline constexpr int FORECAST_DAYS = 5;
inline constexpr int FORECAST_COLUMN_WIDTH = 80;

// Center variable-width text in its forecast column, using a smaller font if needed.
template <typename Display>
void printForecastTemperature(Display &display, const String &text, int columnLeft) {
    int16_t bx, by;
    uint16_t bw, bh;
    display.setFont(&FreeSans12pt7b);
    display.getTextBounds(text, 0, 0, &bx, &by, &bw, &bh);
    if (bw > 76) {
        display.setFont(&FreeSans9pt7b);
        display.getTextBounds(text, 0, 0, &bx, &by, &bw, &bh);
    }
    display.setCursor(columnLeft + (FORECAST_COLUMN_WIDTH - bw) / 2 - bx, 235);
    display.print(text);
}

template <typename Display>
void printRainEstimate(Display &display, const char *value, int columnLeft, uint16_t color) {
    const bool available = value[0] != '\0';
    const String text = available ? "~" + String(atof(value), 0) + "%" : String("--");
    int16_t bx, by;
    uint16_t bw, bh;
    display.setFont(&FreeSans9pt7b);
    display.getTextBounds(text, 0, 0, &bx, &by, &bw, &bh);
    const int iconWidth = available ? 8 : 0;
    const int gap = available ? 4 : 0;
    const int left = columnLeft + (FORECAST_COLUMN_WIDTH - iconWidth - gap - bw) / 2;
    if (available) {
        // Droplet plus '~' marks an estimated chance of rain, not an API probability.
        display.fillTriangle(left + 4, 284, left, 291, left + 8, 291, color);
        display.fillCircle(left + 4, 292, 4, color);
    }
    display.setCursor(left + iconWidth + gap - bx, 297);
    display.print(text);
}

template <typename Display, typename Prediction, typename IconFor>
void printForecast(Display &display, const Prediction &weatherPrediction,
                   IconFor iconFor, uint16_t color) {
    display.drawLine(0, 180, 400, 180, color);
    display.drawLine(0, 181, 400, 181, color); // bigger 2 px line

    for (int i = 0; i < FORECAST_DAYS; i++) {
        const int columnLeft = FORECAST_COLUMN_WIDTH * i;
        const int x_cursor = columnLeft + 10;
        const int y_cursor = 205;

        const auto &day = weatherPrediction.fiveDays[i];
        const bool dayAvailable = day.day[0] != '\0';
        const String maxTemp = day.maxTemp[0] ? String(atof(day.maxTemp), 0) : String("--");
        const String minTemp = day.minTemp[0] ? String(atof(day.minTemp), 0) : String("--");

        // Day
        display.setCursor(x_cursor, y_cursor);
        display.setFont(&FreeSansBold9pt7b);
        display.print(dayAvailable ? day.day : "--");

        // Max / min temp
        printForecastTemperature(display, maxTemp + "/" + minTemp, columnLeft);

        // Icon
        display.drawBitmap(x_cursor + 7, 240, iconFor(dayAvailable ? day.iconCode : 0), 40, 40, color);

        printRainEstimate(display, dayAvailable ? day.rainProbability : "", columnLeft, color);
    }
}

} // namespace meteo_station_common
