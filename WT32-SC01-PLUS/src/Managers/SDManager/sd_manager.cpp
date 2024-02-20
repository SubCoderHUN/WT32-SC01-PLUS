#include "SPI.h"
#include "sd_manager.h"
#include "../../main.h"

const char *path = "/log.txt";  //  File for logging

void SDSetup()
{
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  SPI.begin(SDMMC_CLK, SDMMC_D0, SDMMC_CMD);
  SD.begin(SD_CS);

  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE)
  {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC)
    Serial.println("MMC");
  else if (cardType == CARD_SD)
    Serial.println("SDSC");
  else if (cardType == CARD_SDHC)
    Serial.println("SDHC");
  else
    Serial.println("UNKNOWN");

  Serial.print("SD Card size: ");
  Serial.print(SD.cardSize() / (1024 * 1024));
  Serial.print("\n");
  Serial.print("SD Card used bytes: ");
  Serial.print(SD.usedBytes() / (1024 * 1024));
  Serial.print("\n");
}
fs::FS SD_FS(fs::FS &fs)
{
  return fs;
}
void SDwriteFile()
{
  if (SDCARD_INSERTED)
  {
    if (!SD.exists(path))
    {
      Serial.printf("Writing file: %s\n", path);
      File file = SD_FS(SD).open(path, FILE_WRITE);
      if (!file)
      {
        Serial.println("Failed to open file for writing");
        return;
      }
      if (file.print("\n*-*-*-*-*-*-*-*-*-* LOG STARTED *-*-*-*-*-*-*-*-*-*\n"))
        Serial.println("File written");
      else
        Serial.println("Write failed");
      file.close();
    }
    else
      SD_LOG("\n*-*-*-*-*-*-*-*-*-* LOG STARTED *-*-*-*-*-*-*-*-*-*\n");
  }
}
void SD_LOG(const char *message)
{
  Serial.write(message);
  if (SDCARD_INSERTED)
  {
    File file = SD_FS(SD).open(path, FILE_APPEND);
    if (!file)
    {
      Serial.write("Could not append to file ");
      Serial.write(path);
      Serial.write("\n");
      return;
    }
    file.print(message);
    file.close();
  }
}