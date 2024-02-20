#include <Arduino.h>
#include "Audio.h"

extern Audio audio;

extern bool radioIsPlaying;

extern void StartStopRadio();
extern void radiotask(void *);
extern void ReadRadFromEEPROM();
extern void SetupRadio();
extern void InitRadioPinout();