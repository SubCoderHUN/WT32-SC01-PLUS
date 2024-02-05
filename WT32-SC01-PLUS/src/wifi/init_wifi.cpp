#include "WiFi.h"
#include "init_wifi.h"
#include "EEPROMManager/eeprom_manager.h"
#include "ui.h"

int ccount, eepromccount = 0;
bool isConnected = false;
bool runState = false;
int forOne, forTwo, forThree = 0;
bool stoprun = false;
int networks = 0;
bool totalstop = false;

void WifiOff()
{
    WiFi.disconnect(true, true);
    WiFi.mode(WIFI_OFF);
}
bool StartWifiFromEEPROM()
{
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    if (WiFi.status() != WL_CONNECTED)
    {
        while (WiFi.status() != WL_CONNECTED && eepromccount != 10)
        {
            delay(1000);
            eepromccount++;
            Serial.write("\nConnecting to WiFi...");
            Serial.print(eepromccount);
            Serial.write("\n");
        }
    }
    if ((WiFi.status() != WL_CONNECTED && eepromccount == 10) || WiFi.status() == WL_CONNECT_FAILED || WiFi.status() == WL_CONNECTION_LOST)
    {
        Serial.write("\nCan't Connect to WiFi!\n");
        lv_obj_add_flag(ui_wifionimg, LV_OBJ_FLAG_HIDDEN);    // Hide connected icon
        lv_obj_clear_flag(ui_wifioffimg, LV_OBJ_FLAG_HIDDEN); // Show disconnected icon
        lv_obj_add_flag(ui_wifionoptimg, LV_OBJ_FLAG_HIDDEN); // Hide connected icon in options screen
        eepromccount = 0;
        isConnected = false;
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.write("\nConnected to the WiFi network!\n");
        lv_obj_clear_flag(ui_wifionimg, LV_OBJ_FLAG_HIDDEN);    // Show connected icon
        lv_obj_add_flag(ui_wifioffimg, LV_OBJ_FLAG_HIDDEN);     // Hide disconnected icon
        lv_obj_clear_flag(ui_wifionoptimg, LV_OBJ_FLAG_HIDDEN); // Hide connected icon in options screen
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
    WiFi.begin(lv_textarea_get_text(ui_wifissidarea), lv_textarea_get_text(ui_wifipassarea));
    if (WiFi.status() != WL_CONNECTED)
    {
        while (WiFi.status() != WL_CONNECTED && ccount != 10)
        {
            delay(1000);
            ccount++;
            Serial.write("\nConnecting to WiFi...");
            Serial.print(ccount);
            Serial.write("\n");
        }
    }
    if ((WiFi.status() != WL_CONNECTED && ccount == 10) || WiFi.status() == WL_CONNECT_FAILED || WiFi.status() == WL_CONNECTION_LOST)
    {
        Serial.write("\nCan't Connect to WiFi!\n");
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
        Serial.write("\nConnected to the WiFi network!\n");
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
        ccount = 0;
        isConnected = true;
        WIFI_SSID = lv_textarea_get_text(ui_wifissidarea);
        WIFI_PASS = lv_textarea_get_text(ui_wifipassarea);
        writeStringToEEPROM(0, WIFI_SSID);  // Write SSID to EEPROM
        writeStringToEEPROM(64, WIFI_PASS); // Write password to EEPROM
        EndWriting();                       // End writing to EEPROM
    }
    return isConnected;
}
void ChangeWifiState(bool state)
{
    if (state)
    {
        for (forOne; forOne < 1; forOne++)
        {
            Serial.write("WiFi turned ON.\n");
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
            Serial.write("WiFi turned OFF.\n");
            lv_obj_add_flag(ui_handshakebtn, LV_OBJ_FLAG_HIDDEN); // Hide connect icon
            lv_obj_add_flag(ui_wifipassarea, LV_OBJ_FLAG_HIDDEN); // Hide passarea
            lv_obj_add_flag(ui_wifissidarea, LV_OBJ_FLAG_HIDDEN); // Hide ssidarea
            lv_obj_add_flag(ui_keyboard, LV_OBJ_FLAG_HIDDEN);     // Hide keyboard
        }
    }
}