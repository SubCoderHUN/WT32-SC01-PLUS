extern bool isConnected;
extern int eepromccount;

void WifiOff();
bool InitWifi();
void ChangeWifiState(bool);
bool StartWifiFromEEPROM();