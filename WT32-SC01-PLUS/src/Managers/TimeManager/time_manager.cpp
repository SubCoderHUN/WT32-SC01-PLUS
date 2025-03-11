#include <Arduino.h>
#include <ESP32Time.h>
#include "time_manager.h"
#include "../WiFiManager/wifi_manager.h"
#include "../SDManager/sd_manager.h"

// Timezone settings
const int GMT_OFFSET = 3600; // Base GMT + 1 hour (winter time)
const int DST_OFFSET = 3600; // Daylight saving time offset (1 hour)

tm timeinfo;
ESP32Time rtc(0); // Initialize RTC with GMT+0
String hour = "00";
String minute = "00";
String date = "2024.04.01";

// Checks if the time is valid
void checkTime() {
    if (!getLocalTime(&timeinfo)) {
        SD_LOG("Failed to obtain time");
        return;
    }
}

// Formats the time and date
void trimTimeDate() {
    char buffer[25]; // Buffer for formatting time and date

    // Format hour
    snprintf(buffer, sizeof(buffer), "%02d", rtc.getHour(true));
    hour = buffer;

    // Format minute
    snprintf(buffer, sizeof(buffer), "%02d", rtc.getMinute());
    minute = buffer;

    // Format date
    snprintf(buffer, sizeof(buffer), "%04d.%02d.%02d", rtc.getYear(), rtc.getMonth() + 1, rtc.getDay());
    date = buffer;
}

// Automatically handles daylight saving time
int getTimezoneOffset() {
    // Daylight saving time: from the last Sunday of March to the last Sunday of October
    // Create a time structure with the current date
    time_t now = time(nullptr);
    tm* tm = localtime(&now);

    // Check if daylight saving time is active
    if (tm->tm_mon > 2 && tm->tm_mon < 9) { // April 1 to September 30
        return GMT_OFFSET + DST_OFFSET; // Summer time: GMT + 2 hours
    } else if (tm->tm_mon == 2 && tm->tm_mday - tm->tm_wday >= 25) { // Last Sunday of March
        return GMT_OFFSET + DST_OFFSET; // Summer time: GMT + 2 hours
    } else if (tm->tm_mon == 9 && tm->tm_mday - tm->tm_wday < 25) { // Last Sunday of October
        return GMT_OFFSET + DST_OFFSET; // Summer time: GMT + 2 hours
    } else {
        return GMT_OFFSET; // Winter time: GMT + 1 hour
    }
}

// Initializes the time
void InitTime() {
    if (isConnected) {
        int offset = getTimezoneOffset(); // Automatically calculate timezone offset
        SD_LOG("Configuring time for timezone offset: " + offset);
        configTime(offset, 0, NTP_SERVERS); // Set the timezone
        SD_LOG("Updating time...");
        checkTime();
        SD_LOG("Time & Date synchronized!");
        trimTimeDate();
        rtc.setTimeStruct(timeinfo);
    }
}