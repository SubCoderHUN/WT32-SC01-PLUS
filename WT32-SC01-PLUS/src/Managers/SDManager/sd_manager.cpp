#include "SPI.h"
#include "sd_manager.h"
#include "../../main.h"

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
  {
    Serial.println("MMC");
  }
  else if (cardType == CARD_SD)
  {
    Serial.println("SDSC");
  }
  else if (cardType == CARD_SDHC)
  {
    Serial.println("SDHC");
  }
  else
  {
    Serial.println("UNKNOWN");
  }
}
void SDwriteFile(fs::FS &fs, const char *path)
{
  if (SDCARD_INSERTED)
  {
    if (!SD.exists(path))
    {
      Serial.printf("Writing file: %s\n", path);
      File file = fs.open(path, FILE_WRITE);
      if (!file)
      {
        Serial.println("Failed to open file for writing");
        return;
      }
      if (file.print("*-*-*-*-*-*-*-*-*-* LOG STARTED *-*-*-*-*-*-*-*-*-*\n"))
        Serial.println("File written");
      else
        Serial.println("Write failed");
      file.close();
    }
    else
      SD_LOG("*-*-*-*-*-*-*-*-*-* LOG STARTED *-*-*-*-*-*-*-*-*-*\n");
  }
}
fs::FS SD_FS(fs::FS &fs)
{
  return fs;
}
void SD_LOG(const char *message)
{
  if (SDCARD_INSERTED)
  {
    Serial.write(message);
    File file = SD_FS(SD).open("/log.txt", FILE_APPEND);
    if (!file)
      return;
    file.print(message);
    file.close();
  }
  else
    Serial.write(message);
}