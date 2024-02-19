#include "SD.h"
#include "FS.h"

#define SD_CS 41
#define SDMMC_CMD 40
#define SDMMC_CLK 39
#define SDMMC_D0 38

fs::FS SD_FS(fs::FS &);
void SDSetup();
void SDwriteFile();
void SD_LOG(const char * );