#include "SD.h"
#include "FS.h"

#define SD_CS 41
#define SDMMC_CMD 40
#define SDMMC_CLK 39
#define SDMMC_D0 38


void SDSetup();
void SDwriteFile(fs::FS &, const char *);
fs::FS SD_FS(fs::FS &);
void SD_LOG(const char * );