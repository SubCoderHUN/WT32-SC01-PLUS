#include "eeprom_manager.h"
#include <EEPROM.h>
#include <Preferences.h>
#include "../SDManager/sd_manager.h"

/*
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
            STORED SETTINGS IN EEPROM
                0   -   WIFI_SSID
                64  -   WIFI_PASS
                100 -   SCREEN BRIGHTNESS
                120 -   LOCATION
                150 -   DIMMING FROM BRIGHTNESS
                160 -   DIMMING TO BRIGHNESS
                165 -   RADIO PLAYING STATUS
                170 -   RADIO STATION INDEX
                175 -   RADIO VOLUME LEVEL
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
*/

String WIFI_SSID;
String WIFI_PASS;
bool WiFiCredIsSaved;

void StartWriting()
{
    if (!EEPROM.begin(512)) // Initialise EEPROM
        SD_LOG("EEPROM failed to initialise\n");
}
void EndWriting()
{
    EEPROM.end();
}
void writeIntInToEEPROM(int address, int number)
{
    StartWriting();
    SD_LOG("Writing (INT) ");
    SD_LOG(((String)number).c_str());
    SD_LOG("to address: ");
    SD_LOG(((String)address).c_str());
    SD_LOG("\n");
    EEPROM.write(address, number);
    EndWriting();
}
int readIntFromEEPROM(int address)
{
    StartWriting();
    SD_LOG("Reading (INT) from address: ");
    SD_LOG(((String)address).c_str());
    SD_LOG("\n");
    return EEPROM.read(address);
    EndWriting();
}
void writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
    StartWriting();
    SD_LOG("Writing (STRING) ");
    SD_LOG(strToWrite.c_str());
    SD_LOG(" to address: ");
    SD_LOG(((String)addrOffset).c_str());
    SD_LOG("\n");
    byte len = strToWrite.length();
    EEPROM.write(addrOffset, len);
    for (int i = 0; i < len; i++)
    {
        EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
    }
    EndWriting();
}
String readStringFromEEPROM(int addrOffset)
{
    StartWriting();
    SD_LOG("Reading (STRING) from address: ");
    SD_LOG(((String)addrOffset).c_str());
    SD_LOG("\n");
    int newStrLen = EEPROM.read(addrOffset);
    char data[newStrLen + 1];
    for (int i = 0; i < newStrLen; i++)
    {
        data[i] = EEPROM.read(addrOffset + 1 + i);
    }
    data[newStrLen] = '\0';
    return String(data);
    EndWriting();
}
void EraseEEPROM()
{
    for (int i = 0; i < 512; i++)
    {
        EEPROM.write(i, 0);
    }
    EEPROM.commit();
}
bool CheckEEPROMAddress(int address)
{
    if (!EEPROM.begin(512)) // Initialise EEPROM
    {
        SD_LOG("EEPROM failed to initialise\n");
        return false;
    }
    else
    {
        if (EEPROM.read(address) == 255) // EMPTY
        {
            SD_LOG("\nAddress: ");
            SD_LOG(((String)address).c_str());
            SD_LOG(" is empty in EEPROM.\n");
            EndWriting();
            return false;
        }
        else // Has content
        {
            SD_LOG("\nAddress: ");
            SD_LOG(((String)address).c_str());
            SD_LOG(" is exist in EEPROM.\n");
            EndWriting();
            return true;
        }
    }
}