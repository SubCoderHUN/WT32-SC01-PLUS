extern bool isConnected;
extern int eepromccount;
extern bool connectOverride;

void InitWOL();
void RunWOL();
void WiFiErrorHandling(void *); // Check if everything is ok with the wifi connection
bool InitWifi();
void ChangeWifiState(bool);
bool StartWifiFromEEPROM();