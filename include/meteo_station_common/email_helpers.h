#pragma once

#include <Arduino.h>

namespace meteo {

inline String htmlEscape(const String& in) {
    String out;
    out.reserve(in.length() + 16);
    for (size_t i = 0; i < in.length(); i++) {
        const char c = in[i];
        switch (c) {
            case '&': out += F("&amp;"); break;
            case '<': out += F("&lt;"); break;
            case '>': out += F("&gt;"); break;
            case '"': out += F("&quot;"); break;
            case '\'': out += F("&#39;"); break;
            default: out += c; break;
        }
    }
    return out;
}

inline String trimCopy(const String& s) {
    int start = 0;
    while (start < static_cast<int>(s.length()) && isspace(static_cast<unsigned char>(s[start]))) start++;

    int end = static_cast<int>(s.length()) - 1;
    while (end >= start && isspace(static_cast<unsigned char>(s[end]))) end--;

    if (end < start) return "";
    return s.substring(start, end + 1);
}

inline void appendBullet(String& html, const String& itemRaw) {
    const String item = trimCopy(itemRaw);
    if (item.length() == 0) return;

    html += F("<li>");
    html += htmlEscape(item);
    html += F("</li>");
}

inline String commitToHtmlBullets(const String& commitMsg) {
    String out;
    out.reserve(commitMsg.length() + 64);

    String token;
    token.reserve(64);

    for (size_t i = 0; i < commitMsg.length(); i++) {
        const char c = commitMsg[i];
        if (c == '\r') continue;

        if (c == '\n' || c == ',') {
            appendBullet(out, token);
            token = "";
            continue;
        }

        token += c;
    }

    appendBullet(out, token);
    return out;
}

} // namespace meteo
