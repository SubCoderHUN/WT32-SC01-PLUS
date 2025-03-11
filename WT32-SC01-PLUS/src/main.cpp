/////////////////////////////////////////////////////////////////
/*
  Erase Flash: esptool.py --chip esp32-s3 erase_flash

  °°°°°°°°°°°°°°°° Handcrafted and made with ❤ °°°°°°°°°°°°°°°°
*/
/////////////////////////////////////////////////////////////////

#include <Wire.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <lvgl.h>
#include <ESP32Time.h>
#include "Audio.h"
#include "ui.h"
#include "Managers/TimeManager/time_manager.h"
#include "Managers/WiFiManager/wifi_manager.h"
#include "Managers/EEPROMManager/eeprom_manager.h"
#include "Managers/SDManager/sd_manager.h"
#include "Features/weather/weather.h"
#include "Features/radio/radio.h"
#include "main.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SDCARD_INSERTED = false; //  Change this to false, if you are not using an SD card to save the logs!
////////////////////////////////////////////////////////////////////////////////////////////////////////

SimpleTimer Timer;     // ~For weather update, 5 min
SimpleTimer WiFiTimer; // ~For WiFi management

static const int RXPin = 10, TXPin = 11, sclPin = 12, sdaPin = 13;
bool isSetupCompleted, runOnce, runWifiState, WriteLocToEEPROMOnce, doonce, setbrightness, setbrightnessbytime, setbrightness_s, setbrightnessbytime_s = false;
int screen_brightness = 255;
int fromroller, toroller = 0;
int forint = 0;
char location[15];

class LGFX : public lgfx::LGFX_Device
{
  lgfx::Panel_ST7796 _panel_instance;
  lgfx::Bus_Parallel8 _bus_instance;
  lgfx::Light_PWM _light_instance;
  lgfx::Touch_FT5x06 _touch_instance;

public:
  LGFX(void)
  {
    {
      auto cfg = _bus_instance.config();
      cfg.freq_write = 40000000;
      cfg.pin_wr = 47;
      cfg.pin_rd = -1;
      cfg.pin_rs = 0;
      cfg.pin_d0 = 9;
      cfg.pin_d1 = 46;
      cfg.pin_d2 = 3;
      cfg.pin_d3 = 8;
      cfg.pin_d4 = 18;
      cfg.pin_d5 = 17;
      cfg.pin_d6 = 16;
      cfg.pin_d7 = 15;
      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }

    {
      auto cfg = _panel_instance.config();
      cfg.pin_cs = -1;
      cfg.pin_rst = 4;
      cfg.pin_busy = -1;
      cfg.memory_width = 320;
      cfg.memory_height = 480;
      cfg.panel_width = 320;
      cfg.panel_height = 480;
      cfg.offset_x = 0;
      cfg.offset_y = 0;
      cfg.offset_rotation = 0;
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits = 1;
      cfg.readable = true;
      cfg.invert = true;
      cfg.rgb_order = false;
      cfg.dlen_16bit = false;
      cfg.bus_shared = true;

      _panel_instance.config(cfg);
    }

    {
      auto cfg = _light_instance.config();
      cfg.pin_bl = 45;
      cfg.invert = false;
      cfg.freq = 44100;
      cfg.pwm_channel = 7;

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);
    }

    {
      auto cfg = _touch_instance.config();
      cfg.i2c_port = 1;
      cfg.i2c_addr = 0x38;
      cfg.pin_sda = 6;
      cfg.pin_scl = 5;
      cfg.freq = 400000;
      cfg.x_min = 0;
      cfg.x_max = 320;
      cfg.y_min = 0;
      cfg.y_max = 480;

      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);
    }

    setPanel(&_panel_instance);
  }
};
static LGFX tft;

/*Change to your screen resolution*/
static const uint32_t screenWidth = 480;
static const uint32_t screenHeight = 320;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 100];

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.writePixels((lgfx::rgb565_t *)&color_p->full, w * h);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{

  uint16_t x, y;
  if (tft.getTouch(&x, &y))
  {
    data->state = LV_INDEV_STATE_PR;
    data->point.x = x;
    data->point.y = y;
  }
  else
  {
    data->state = LV_INDEV_STATE_REL;
  }
}

void initDisplay()
{
  tft.begin();
  tft.setRotation(1);
  tft.setBrightness(screen_brightness);
  tft.fillScreen(TFT_BLACK);
}
static void separator_blink_timer_cb(lv_timer_t * timer) //Blink the separator
{
    static bool is_visible = true;

    if (is_visible) {
        lv_obj_add_flag(ui_separator, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_clear_flag(ui_separator, LV_OBJ_FLAG_HIDDEN);
    }

    is_visible = !is_visible; // Change the visibility
}
void DisplayTime()
{
  trimTimeDate();                                       // Trim 1,2,3 to 01,02,03...
  lv_label_set_text(ui_hour, String(hour).c_str());     // Change hour
  lv_label_set_text(ui_minute, String(minute).c_str()); // Change minute
  lv_label_set_text(ui_datetext, String(date).c_str()); // Change date
}
void WifiSettings()
{
  if (lv_obj_has_state(ui_Wifisw, LV_STATE_CHECKED))
    ChangeWifiState(true); // Turn WiFi ON
}
void KeyboardManagement()
{
  if (lv_obj_has_state(ui_wifipassarea, LV_EVENT_CLICKED)) // Keyboard stuff
    lv_keyboard_set_textarea(ui_keyboard, ui_wifipassarea);
  if (lv_obj_has_state(ui_wifissidarea, LV_EVENT_CLICKED)) // Keyboard stuff
    lv_keyboard_set_textarea(ui_keyboard, ui_wifissidarea);
}
void RadioFix()
{
  if (radioIsPlaying)
    delay(30); // 30ms delay for the radio, 5ms is too short, the datastream is sutters
  else
    delay(5);
}
void BrightnessControll()
{
  if (lv_slider_is_dragged(ui_brightnessslider)) // Screen brightness settings
  {
    if (lv_slider_get_value(ui_brightnessslider) > 0)
    {
      lv_obj_clear_flag(ui_percentagetext1, LV_OBJ_FLAG_HIDDEN);
      char brightness_char[10];                                                          // Buffer big enough for 7-character float
      dtostrf((lv_slider_get_value(ui_brightnessslider) / 2.55), 6, 0, brightness_char); // Leave room for too large numbers!
      lv_label_set_text(ui_BrightnessText, brightness_char);
      tft.setBrightness(lv_slider_get_value(ui_brightnessslider));
      screen_brightness = lv_slider_get_value(ui_brightnessslider);
      SD_LOG("\nWriting brightness to EEPROM...");
      writeIntInToEEPROM(100, screen_brightness); // Write screem brightness to EEPROM
    }
    else
    {
      lv_obj_clear_flag(ui_percentagetext1, LV_OBJ_FLAG_HIDDEN);
      lv_slider_set_value(ui_brightnessslider, 2, LV_ANIM_ON);
      char brightness_char[10];                                                          // Buffer big enough for 7-character float
      dtostrf((lv_slider_get_value(ui_brightnessslider) / 2.55), 6, 0, brightness_char); // Leave room for too large numbers!
      lv_label_set_text(ui_BrightnessText, brightness_char);
    }
  }
}
void SetBrightnessDim()
{
  tft.setBrightness(1);
  lv_slider_set_value(ui_brightnessslider, 1, LV_ANIM_OFF);
  lv_label_set_text(ui_BrightnessText, "Auto");
  lv_obj_add_flag(ui_percentagetext1, LV_OBJ_FLAG_HIDDEN);
}
void SetBrightnessNormal()
{
  tft.setBrightness(screen_brightness);
  lv_slider_set_value(ui_brightnessslider, screen_brightness, LV_ANIM_OFF);
  lv_label_set_text(ui_BrightnessText, "Auto");
  lv_obj_add_flag(ui_percentagetext1, LV_OBJ_FLAG_HIDDEN);
}
void BrightnessDIMMControll()
{
  if (lv_roller_get_selected(ui_brrollerfrom) != fromroller) //  Screen dimming - from roller
  {
    fromroller = lv_roller_get_selected(ui_brrollerfrom);
    writeIntInToEEPROM(150, fromroller);
  }
  if (lv_roller_get_selected(ui_brrollerto) != toroller) //  Screen dimming - to roller
  {
    toroller = lv_roller_get_selected(ui_brrollerto);
    writeIntInToEEPROM(160, toroller);
  }
  if (fromroller != toroller)
  {
    if (toroller < fromroller) //  Var.1
    {
      setbrightness_s = false;
      setbrightnessbytime_s = true;
      if (((hour.toInt() > toroller && hour.toInt() >= fromroller) && (hour.toInt() != 0)) || ((hour.toInt() < toroller && hour.toInt() <= fromroller) && (hour.toInt() >= 0)))
      {
        if (!setbrightnessbytime)
        {
          SetBrightnessDim();
          setbrightness = false;
          setbrightnessbytime = true;
        }
      }
      else
      {
        if (!setbrightness)
        {
          SetBrightnessNormal();
          setbrightnessbytime = false;
          setbrightness = true;
        }
      }
      if (toroller > fromroller) // Var.2
      {
        setbrightness = false;
        setbrightnessbytime = true;
        if (hour.toInt() >= fromroller && hour.toInt() < toroller)
        {
          if (!setbrightnessbytime_s)
          {
            SetBrightnessDim();
            setbrightness_s = false;
            setbrightnessbytime_s = true;
          }
        }
        else
        {
          if (!setbrightness_s)
          {
            SetBrightnessNormal();
            setbrightnessbytime_s = false;
            setbrightness_s = true;
          }
        }
      }
    }
    else
    {
      setbrightness = false;
      setbrightnessbytime = false;
      setbrightness_s = false;
      setbrightnessbytime_s = false;
    }
  }
}
void LocationControll()
{
  if (lv_dropdown_is_open(ui_LocationDropdown))
    WriteLocToEEPROMOnce = false;
  else
  {
    if (!WriteLocToEEPROMOnce)
    {
      lv_dropdown_get_selected_str(ui_LocationDropdown, location, sizeof(location));
      lv_label_set_text(ui_locationtext, location);
      SD_LOG("\nWriting location to EEPROM...\n");
      writeStringToEEPROM(120, location); // Write location to EEPROM
      if (isConnected)
        InitWeather(); // Update weather data
      WriteLocToEEPROMOnce = true;
    }
  }
}
void WeatherTiming()
{
  if (Timer.isReady() && isConnected)
  {
    InitWeather();
    Timer.reset();
  }
}
void RestoreFromEEPROM()
{
  // Note: Full brightness is 255, so we cant check that address in EEPROM. 255 is 0xFF, which will return to empty.
  SD_LOG("Reading brightness from EEPROM...\n"); // Get saved brightness
  int tempbrightness = readIntFromEEPROM(100);
  if (tempbrightness >= 1)
  {
    tft.setBrightness(tempbrightness);
    lv_slider_set_value(ui_brightnessslider, tempbrightness, LV_ANIM_OFF);
    char brightness_char[4];                                                           // Buffer big enough for 5-character float
    dtostrf((lv_slider_get_value(ui_brightnessslider) / 2.55), 6, 0, brightness_char); // Leave room for too large numbers!
    lv_label_set_text(ui_BrightnessText, brightness_char);
    screen_brightness = tempbrightness;
  }
  else
    tft.setBrightness(255);

  if (CheckEEPROMAddress(120)) // Get saved location
  {
    SD_LOG("Reading location from EEPROM...\n");
    String location = readStringFromEEPROM(120);
    if (location.indexOf("Rackeve") == 0)
    {
      lv_dropdown_set_selected(ui_LocationDropdown, 0);
      lv_label_set_text(ui_locationtext, location.c_str());
    }
    if (location.indexOf("Budapest") == 0)
    {
      lv_dropdown_set_selected(ui_LocationDropdown, 1);
      lv_label_set_text(ui_locationtext, location.c_str());
    }
    if (location.indexOf("Kiskunlachaza") == 0)
    {
      lv_dropdown_set_selected(ui_LocationDropdown, 2);
      lv_label_set_text(ui_locationtext, location.c_str());
    }
  }
  if (CheckEEPROMAddress(0)) // Get saved wifi credentials
  {
    bool WiFiCredIsSaved = true;
    SD_LOG("Reading Wifi credentials from EEPROM...\n");
    WIFI_SSID = readStringFromEEPROM(0);
    WIFI_PASS = readStringFromEEPROM(64);
    if (WIFI_SSID != "")
      if (StartWifiFromEEPROM())
      {
        InitTime();
        SetupRadio(); // Setup radio stations for dropdown menu
      }
  }
  if (CheckEEPROMAddress(150)) // Get saved brightness informations for dimming
  {
    if (CheckEEPROMAddress(160)) // Get saved brightness informations for dimming
    {
      fromroller = readIntFromEEPROM(150);
      toroller = readIntFromEEPROM(160);
      lv_roller_set_selected(ui_brrollerfrom, fromroller, LV_ANIM_OFF);
      lv_roller_set_selected(ui_brrollerto, toroller, LV_ANIM_OFF);
      if (fromroller == toroller)
        tft.setBrightness(tempbrightness);
      else
        BrightnessDIMMControll();
    }
  }
}
void setup()
{
  Serial.begin(115200);
  InitRadioPinout();
  initDisplay();
  SDSetup();     //  Setup SD card
  SDwriteFile(); //  Create log.txt
  lv_init();
  lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 100);
  lv_timer_create(separator_blink_timer_cb, 500, NULL); //Timer to blink the separator.

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /*Initialize the (dummy) input device driver*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  ui_init();
  xTaskCreatePinnedToCore(WiFiErrorHandling,
                          "WiFiErrorHandling",
                          4000,
                          NULL,
                          0,
                          NULL,
                          0);
  RestoreFromEEPROM();  //  Restore saved values from EEPROM
  ReadRadFromEEPROM();
  InitWOL();
  Timer.setInterval(300000);   // ~For weather update, 5 min 300000
  WiFiTimer.setInterval(5000); // ~For WiFiErrorHandling task
  
  isSetupCompleted = true;
}
void loop()
{
  lv_timer_handler();
  if (lv_obj_has_state(ui_wifionimg, LV_EVENT_CLICKED) && isConnected)
  {
    RunWOL();
    lv_obj_clear_state(ui_wifionimg, LV_EVENT_CLICKED);
  }
  if (lv_obj_has_state(ui_handshakebtn, LV_EVENT_CLICKED) && !isConnected)
  {
    if (InitWifi())
    {
      InitTime();
      InitWeather();
    }
    lv_obj_clear_state(ui_handshakebtn, LV_EVENT_CLICKED);
  }
  DisplayTime();
  KeyboardManagement();
  BrightnessControll();
  LocationControll();
  WeatherTiming();
  BrightnessDIMMControll();
  WifiSettings();
  RadioFix();
}