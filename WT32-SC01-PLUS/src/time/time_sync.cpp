#include <Arduino.h>
#include <ESP32Time.h>
#include "time_sync.h"
#include "../wifi/init_wifi.h"

int GMT = 3600; // GMT + 1h
tm timeinfo;
ESP32Time rtc(0); // offset in seconds GMT+1
String hour = "00";
String minute = "00";
String date = "2024.02.02";

void checkTime()
{
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
        return;
    }
}
void trimTimeDate()
{

    switch (rtc.getHour(true))
    {
    case 0:
    {
        hour = "00";
        break;
    }
    case 1:
    {
        hour = "01";
        break;
    }
    case 2:
    {
        hour = "02";
        break;
    }
    case 3:
    {
        hour = "03";
        break;
    }
    case 4:
    {
        hour = "04";
        break;
    }
    case 5:
    {
        hour = "05";
        break;
    }
    case 6:
    {
        hour = "06";
        break;
    }
    case 7:
    {
        hour = "07";
        break;
    }
    case 8:
    {
        hour = "08";
        break;
    }
    case 9:
    {
        hour = "09";
        break;
    }
    default:
    {
        hour = rtc.getHour(true);
        break;
    }
    }

    switch (rtc.getMinute())
    {
    case 0:
    {
        minute = "00";
        break;
    }
    case 1:
    {
        minute = "01";
        break;
    }
    case 2:
    {
        minute = "02";
        break;
    }
    case 3:
    {
        minute = "03";
        break;
    }
    case 4:
    {
        minute = "04";
        break;
    }
    case 5:
    {
        minute = "05";
        break;
    }
    case 6:
    {
        minute = "06";
        break;
    }
    case 7:
    {
        minute = "07";
        break;
    }
    case 8:
    {
        minute = "08";
        break;
    }
    case 9:
    {
        minute = "09";
        break;
    }
    default:
    {
        minute = rtc.getMinute();
        break;
    }
    }
    date = rtc.getYear();
    date += ".";
    switch (rtc.getMonth())
    {
    case 0:
    {
        date += "01";
        break;
    }
    case 1:
    {
        date += "02";
        break;
    }
    case 2:
    {
        date += "03";
        break;
    }
    case 3:
    {
        date += "04";
        break;
    }
    case 4:
    {
        date += "05";
        break;
    }
    case 5:
    {
        date += "06";
        break;
    }
    case 6:
    {
        date += "07";
        break;
    }
    case 7:
    {
        date += "08";
        break;
    }
    case 8:
    {
        date += "09";
        break;
    }
    default:
    {
        date += rtc.getMonth();
        break;
    }
    }
    date += ".";
    switch (rtc.getDay())
    {
    case 1:
    {
        date += "01";
        break;
    }
    case 2:
    {
        date += "02";
        break;
    }
    case 3:
    {
        date += "03";
        break;
    }
    case 4:
    {
        date += "04";
        break;
    }
    case 5:
    {
        date += "05";
        break;
    }
    case 6:
    {
        date += "06";
        break;
    }
    case 7:
    {
        date += "07";
        break;
    }
    case 8:
    {
        date += "08";
        break;
    }
    case 9:
    {
        date += "09";
        break;
    }
    default:
    {
        date += rtc.getDay();
        break;
    }
    }
}
void InitTime()
{
    if (isConnected)
    {
        Serial.write("Configuring time for timezone:" + GMT);
        configTime(GMT, 3600, NTP_SERVERS);
        Serial.write("\nUpdating time...\n");
        checkTime();
        Serial.write("Time & Date synchronized!\n");
        trimTimeDate();
        rtc.setTimeStruct(timeinfo);
    }
}