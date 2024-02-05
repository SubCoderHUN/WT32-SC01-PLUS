#include "eeprom_manager.h"
#include <EEPROM.h>
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
  EEPROM.write(address, number);
}
int readIntFromEEPROM(int address)
{
return EEPROM.read(address);
}
void writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
    byte len = strToWrite.length();
    EEPROM.write(addrOffset, len);
    for (int i = 0; i < len; i++)
    {
        EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
    }
}
String readStringFromEEPROM(int addrOffset)
{
    int newStrLen = EEPROM.read(addrOffset);
    char data[newStrLen + 1];
    for (int i = 0; i < newStrLen; i++)
    {
        data[i] = EEPROM.read(addrOffset + 1 + i);
    }
    data[newStrLen] = '\0';
    return String(data);
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
            Serial.print(address);;
            Serial.write(" is exist in EEPROM.\n");
            return true;
        }
    }
}