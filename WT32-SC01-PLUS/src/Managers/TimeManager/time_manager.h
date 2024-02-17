#include "time.h"

#define NTP_SERVERS "pool.ntp.org"
extern struct tm timeinfo;
extern ESP32Time rtc;
extern String hour;
extern String minute;
extern String date;

void InitTime();
void trimTimeDate();