#include "eeprom_manager.h"
#include <EEPROM.h>
#include <Preferences.h>

/*
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
            STORED SETTINGS IN EEPROM
                0   -   WIFI_SSID
                64  -   WIFI_PASS
                100 -   SCREEN BRIGHTNESS
                120 -   LOCATION
*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
*/

String WIFI_SSID;
String WIFI_PASS;
bool WiFiCredIsSaved;

void StartWriting()
{
    if (!EEPROM.begin(512)) // Initialise EEPROM
        Serial.write("\nEEPROM failed to initialise");
    else
        Serial.write("\nEEPROM initialised");
}
void EndWriting()
{
    EEPROM.end();
    Serial.write("\nEEPROM closed\n");
}
void writeIntInToEEPROM(int address, int number)
{
    StartWriting();
    EEPROM.write(address, number);
    EndWriting();
}
int readIntFromEEPROM(int address)
{
    StartWriting();
    return EEPROM.read(address);
    EndWriting();
}
void writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
    StartWriting();
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
        Serial.write("\nEEPROM failed to initialise");
    else
    {
        Serial.write("\nEEPROM initialised");
        if (EEPROM.read(address) == 255) // EMPTY
        {
            Serial.write("\nAddress: ");
            Serial.print(address);
            Serial.write(" is empty in EEPROM.\n");
            return false;
        }
        else // Has content
        {
            Serial.write("\nAddress: ");
            Serial.print(address);
            ;
            Serial.write(" is exist in EEPROM.\n");
            return true;
        }
    }
    EndWriting();
}