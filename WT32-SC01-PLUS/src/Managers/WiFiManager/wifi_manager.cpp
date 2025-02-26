#include <WakeOnLan.h>
#include <ESP32Ping.h>
#include "WiFi.h"
#include "wifi_manager.h"
#include "../EEPROMManager/eeprom_manager.h"
#include "ui.h"
#include "Features/radio/radio.h"
#include "main.h"
#include "../SDManager/sd_manager.h"

WiFiUDP UDP;
WakeOnLan WOL(UDP);

bool isConnected, connectOverride = false;
int ccount, eepromccount, forOne, forTwo = 0;
const char *MACAddress = "00:0B:0E:0F:00:ED";

void InitWOL()
{
    WOL.setRepeat(3, 100); // Optional, repeat the packet three times with 100ms between. WARNING delay() is used between send packet function.
}
void RunWOL()
{
    WOL.sendMagicPacket(MACAddress); // Send Wake On Lan packet with the above MAC address. Default to port 9.
    SD_LOG("Magic Packet sent!\n");
}
void LogWiFiDetails() {
    SD_LOG("ip: ");
    SD_LOG(WiFi.localIP().toString().c_str());
    SD_LOG(" gateway: ");
    SD_LOG(WiFi.gatewayIP().toString().c_str());
    SD_LOG(" dns: ");
    SD_LOG(WiFi.dnsIP().toString().c_str());
    SD_LOG("\n\n");
}
void WiFiErrorHandling(void *pvParameters)
{
    while (true)
    {
        if (WiFiTimer.isReady() && isConnected && isSetupCompleted)
        {
            if (!Ping.ping("www.google.com", 3))
            {
                SD_LOG("WiFiErrorHandling: WiFi connection error! Restarting...\n");
                LogWiFiDetails();
                /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
                //  Can't reconnect to the network when the connection is lost.
                //  Thrust me, I tried EVERYTHING.
                //  The only way to reconnect to the network is to restart the ESP32.
                //
                //  This is a weird "bug," and I can't find a proper solution for this issue.
                ESP.restart();
                /*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
            }
            else
                SD_LOG("WiFiErrorHandling: Pinging google.com successful\n");
            WiFiTimer.reset();
        }
    }
}
bool StartWifiFromEEPROM()
{
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    if (WiFi.status() != WL_CONNECTED)
    {
        while (WiFi.status() != WL_CONNECTED && eepromccount <= 20)
        {
            if (eepromccount == 10)
            {
                WiFi.disconnect(true, true); // Switch off the wifi on making 10 attempts and start again.
                WiFi.begin(WIFI_SSID, WIFI_PASS);
            }
            eepromccount++;
            delay(1000);
            char eepromccount_c[5];                      // Buffer big enough for 6-character float
            dtostrf(eepromccount, 6, 0, eepromccount_c); // Leave room for too large numbers!
            SD_LOG("\nConnecting to WiFi...");
            SD_LOG(eepromccount_c);
            SD_LOG("\n");
        }
    }
    if ((WiFi.status() != WL_CONNECTED && eepromccount == 20) || WiFi.status() == WL_CONNECT_FAILED || WiFi.status() == WL_CONNECTION_LOST)
    {
        SD_LOG("\nCan't Connect to WiFi!\n");
        lv_obj_add_flag(ui_wifionimg, LV_OBJ_FLAG_HIDDEN);    // Hide connected icon
        lv_obj_clear_flag(ui_wifioffimg, LV_OBJ_FLAG_HIDDEN); // Show disconnected icon
        lv_obj_add_flag(ui_wifionoptimg, LV_OBJ_FLAG_HIDDEN); // Hide connected icon in options screen
        eepromccount = 0;
        isConnected = false;
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        SD_LOG("\nConnected to the WiFi network!\n");
        lv_obj_clear_flag(ui_wifionimg, LV_OBJ_FLAG_HIDDEN);    // Show connected icon
        lv_obj_add_flag(ui_wifioffimg, LV_OBJ_FLAG_HIDDEN);     // Hide disconnected icon
        lv_obj_clear_flag(ui_wifionoptimg, LV_OBJ_FLAG_HIDDEN); // Hide connected icon in options screen
        LogWiFiDetails();
        eepromccount = 0;
        isConnected = true;
    }
    return isConnected;
}
bool InitWifi()
{
    String ssidarea = lv_textarea_get_text(ui_wifissidarea);
    if (ssidarea.length() < 1)
        return isConnected = false;
    WIFI_SSID = lv_textarea_get_text(ui_wifissidarea);
    WIFI_PASS = lv_textarea_get_text(ui_wifipassarea);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    if (WiFi.status() != WL_CONNECTED)
    {
        while (WiFi.status() != WL_CONNECTED && ccount <= 10)
        {
            ccount++;
            delay(1000);
            char eepromccount_c[5];                      // Buffer big enough for 6-character float
            dtostrf(eepromccount, 6, 0, eepromccount_c); // Leave room for too large numbers!
            SD_LOG("\nConnecting to WiFi...");
            SD_LOG(eepromccount_c);
            SD_LOG("\n");
        }
    }
    if ((WiFi.status() != WL_CONNECTED && ccount == 10) || WiFi.status() == WL_CONNECT_FAILED || WiFi.status() == WL_CONNECTION_LOST)
    {
        SD_LOG("\nCan't Connect to WiFi!\n");
        lv_obj_add_flag(ui_wifionimg, LV_OBJ_FLAG_HIDDEN);    // Hide connected icon
        lv_obj_clear_flag(ui_wifioffimg, LV_OBJ_FLAG_HIDDEN); // Show disconnected icon
        lv_obj_add_flag(ui_wifionoptimg, LV_OBJ_FLAG_HIDDEN); // Hide connected icon in options screen
        lv_label_set_text(ui_wifistatustext, "Can't connect!");
        lv_obj_clear_state(ui_Wifisw, LV_STATE_CHECKED); // Switch wifi button to OFF
        lv_textarea_set_text(ui_wifissidarea, "");       // Set SSID to blank
        lv_textarea_set_text(ui_wifipassarea, "");       // Set PASS to blank
        ccount = 0;
        isConnected = false;
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        SD_LOG("\nConnected to the WiFi network!\n");
        lv_obj_add_flag(ui_handshakebtn, LV_OBJ_FLAG_HIDDEN);   // Hide connect icon
        lv_obj_add_flag(ui_wifipassarea, LV_OBJ_FLAG_HIDDEN);   // Hide passarea
        lv_obj_add_flag(ui_wifissidarea, LV_OBJ_FLAG_HIDDEN);   // Hide ssidarea
        lv_obj_add_flag(ui_keyboard, LV_OBJ_FLAG_HIDDEN);       // Hide keyboard
        lv_obj_clear_flag(ui_wifionimg, LV_OBJ_FLAG_HIDDEN);    // Show connected icon
        lv_obj_add_flag(ui_wifioffimg, LV_OBJ_FLAG_HIDDEN);     // Hide disconnected icon
        lv_obj_clear_flag(ui_wifionoptimg, LV_OBJ_FLAG_HIDDEN); // Hide connected icon in options screen
        lv_label_set_text(ui_wifistatustext, "Connected!");
        lv_textarea_set_text(ui_wifissidarea, ""); // Set SSID to blank
        lv_textarea_set_text(ui_wifipassarea, ""); // Set PASS to blank
        LogWiFiDetails();
        ccount = 0;
        isConnected = true;
        writeStringToEEPROM(0, WIFI_SSID);  // Write SSID to EEPROM
        writeStringToEEPROM(64, WIFI_PASS); // Write password to EEPROM
        if (radioIsPlaying)
            SetupRadio();
    }
    return isConnected;
}
void ChangeWifiState(bool state)
{
    if (state)
    {
        for (forOne; forOne < 1; forOne++)
        {
            SD_LOG("WiFi turned ON.\n");
            lv_label_set_text(ui_wifistatustext, "");
            lv_obj_clear_flag(ui_wifistatustext, LV_OBJ_FLAG_HIDDEN); // Hide wifi status text
            lv_obj_clear_flag(ui_handshakebtn, LV_OBJ_FLAG_HIDDEN);   // Show connect icon
            lv_obj_clear_flag(ui_wifipassarea, LV_OBJ_FLAG_HIDDEN);   // Show passarea
            lv_obj_clear_flag(ui_wifissidarea, LV_OBJ_FLAG_HIDDEN);   // Show ssidarea
            lv_obj_clear_flag(ui_keyboard, LV_OBJ_FLAG_HIDDEN);       // Show keyboard
            InitWifi();
            forTwo = 0;
        }
    }
    else
    {
        for (forTwo; forTwo < 1; forTwo++)
        {
            WiFi.disconnect();
            WiFi.mode(WIFI_OFF);
            forOne = 0;
            SD_LOG("WiFi turned OFF.\n");
            lv_obj_add_flag(ui_handshakebtn, LV_OBJ_FLAG_HIDDEN); // Hide connect icon
            lv_obj_add_flag(ui_wifipassarea, LV_OBJ_FLAG_HIDDEN); // Hide passarea
            lv_obj_add_flag(ui_wifissidarea, LV_OBJ_FLAG_HIDDEN); // Hide ssidarea
            lv_obj_add_flag(ui_keyboard, LV_OBJ_FLAG_HIDDEN);     // Hide keyboard
        }
    }
}