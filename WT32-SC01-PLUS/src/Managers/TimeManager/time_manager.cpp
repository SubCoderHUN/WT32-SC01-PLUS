#include <Arduino.h>
#include <ESP32Time.h>
#include "time_manager.h"
#include "../WiFiManager/wifi_manager.h"
#include "../SDManager/sd_manager.h"

int GMT = 3600; // GMT + 1h
tm timeinfo;
ESP32Time rtc(0); // offset in seconds GMT+0
String hour = "00";
String minute = "00";
String date = "2024.04.01";

void checkTime()
{
    if (!getLocalTime(&timeinfo))
    {
        SD_LOG("Failed to obtain time");
        return;
    }
}
void trimTimeDate() {
    char buffer[15]; // Buffer for formatting time and date

    // Format hour
    snprintf(buffer, sizeof(buffer), "%02d", rtc.getHour(true));
    hour = buffer;

    // Format minute
    snprintf(buffer, sizeof(buffer), "%02d", rtc.getMinute());
    minute = buffer;

    // Format date
    snprintf(buffer, sizeof(buffer), "%04d.%02d.%02d", rtc.getYear(), rtc.getMonth(), rtc.getDay());
    date = buffer;
}
void InitTime()
{
    if (isConnected)
    {
        SD_LOG("Configuring time for timezone:" + GMT);
        configTime(GMT, 3600, NTP_SERVERS);
        SD_LOG("Updating time...");
        checkTime();
        SD_LOG("Time & Date synchronized!");
        trimTimeDate();
        rtc.setTimeStruct(timeinfo);
    }
}