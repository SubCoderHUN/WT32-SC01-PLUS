extern bool isConnected;
extern int eepromccount;

void InitWOL();
void RunWOL();
void WiFiErrorHandling(const char *);
void WifiOff();
bool InitWifi();
void ChangeWifiState(bool);
bool StartWifiFromEEPROM();