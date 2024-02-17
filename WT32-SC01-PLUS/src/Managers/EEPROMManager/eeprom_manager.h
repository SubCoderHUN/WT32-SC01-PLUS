#include <Arduino.h>

extern String WIFI_SSID;
extern String WIFI_PASS;
extern bool WiFiCredIsSaved;

void writeIntInToEEPROM(int, int);
int readIntFromEEPROM(int);
void writeStringToEEPROM(int, const String &);
String readStringFromEEPROM(int);
bool CheckEEPROMAddress(int);
void StartWriting();
void EndWriting();