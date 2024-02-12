#include <Arduino.h>
#include "radio.h"
#include "ui.h"
#include "wifi/init_wifi.h"
// Define I2S connections
#define I2S_LRC 35
#define I2S_BCLK 36
#define I2S_DOUT 37

Audio audio;              // Create audio object
AudioBuffer audio_buffer; // Create audio buffer object
TaskHandle_t *RadioTaskHandle;

int statcount = 0;
int volume = 10;
bool radioIsPlaying, stateisselected = false;
const int ARRAY_SIZE = 7; // Array size for stations
const char *stations[ARRAY_SIZE]{
    "0n-80s.radionetz.de:8000/0n-70s.mp3",       // Radionetz.de
    "icast.connectmedia.hu/5202/live.mp3",       // Radio 1
    "icast.connectmedia.hu/5002/live.mp3",       // Retro
    "icast.connectmedia.hu/4738/mr2.mp3",        // Petőfi
    "stream1.virtualisan.net/prx/8200/live.mp3", // Poptarisznya
    "dancewave.online/dance.mp3",                // Dance Wave
    "s01.diazol.hu:9052/live"                    // Music Station
};
const char *stations_name[ARRAY_SIZE]{
    "Radionetz",
    "Radio 1",
    "Retro",
    "Petofi",
    "Poptarisznya",
    "Dance Wave",
    "Music Station"};

void StartStopRadio()
{
    if (radioIsPlaying)
        audio.connecttohost(stations[statcount]);
    else
        audio.stopSong();
}
void radiotask(void *pvParameters)
{
    while (isConnected)
    {
        if (lv_obj_has_state(ui_playbtn, LV_EVENT_CLICKED))
        {
            radioIsPlaying = true;
            StartStopRadio();
            lv_obj_add_flag(ui_playbtn, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ui_stopbtn, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_state(ui_playbtn, LV_EVENT_CLICKED);
        }
        if (lv_obj_has_state(ui_stopbtn, LV_EVENT_CLICKED))
        {
            radioIsPlaying = false;
            StartStopRadio();
            lv_obj_add_flag(ui_stopbtn, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(ui_playbtn, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_state(ui_stopbtn, LV_EVENT_CLICKED);
        }
        if (lv_dropdown_is_open(ui_radiostationdropdown))
            stateisselected = true;
        else
        {
            if (stateisselected)
            {
                statcount = lv_dropdown_get_selected(ui_radiostationdropdown);
                radioIsPlaying = true;
                StartStopRadio();
                lv_obj_clear_state(ui_stopbtn, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_state(ui_playbtn, LV_OBJ_FLAG_HIDDEN);
                stateisselected = false;
            }
        }
        if (lv_slider_is_dragged(ui_volumeslider))
        {
            volume = lv_slider_get_value(ui_volumeslider);
            audio.setVolume(volume);
        }
        audio.loop();
    }
    vTaskDelete(RadioTaskHandle);
}
void SetupRadio()
{
    lv_slider_set_value(ui_volumeslider,volume,LV_ANIM_OFF);
    audio.setVolume(volume);
    lv_dropdown_clear_options(ui_radiostationdropdown);
    for (int i = 0; i < ARRAY_SIZE; i++) // Setup stations list
        lv_dropdown_add_option(ui_radiostationdropdown, stations_name[i], i);
    xTaskCreatePinnedToCore(radiotask,
                            "radiotask",
                            4000,
                            NULL,
                            1,
                            RadioTaskHandle,
                            1);
    StartStopRadio();
}
void InitRadioPinout()
{
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT); // Set pinout for speaker
}