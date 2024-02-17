#include <JsonListener.h>
#include "weather.h"
#include "ui.h"
#include "Managers/SDManager/sd_manager.h"

OpenWeatherMapCurrent client;
String OPEN_WEATHER_MAP_APP_ID = "364666b6317549177d48dca30b6da114"; // OpenWeatherMap API key
String OPEN_WEATHER_MAP_LOCATION_ID_RACKEVE = "3046026";             // Location: Ráckeve
String OPEN_WEATHER_MAP_LOCATION_ID_BUDAPEST = "3054643";            // Location: Budapest
String OPEN_WEATHER_MAP_LOCATION_ID_KISKUNLACHAZA = "3049878";       // Location: Kiskunlacháza
String OPEN_WEATHER_MAP_LANGUAGE = "en";
boolean IS_METRIC = true;

String StringToUpperCase(String str)
{
    str[0] = std::toupper(str[0]);
    for (std::size_t i = 1; i < str.length(); ++i)
        str[i] = std::tolower(str[i]);
    return str;
}
void InitWeather()
{
    Serial.write("\nUpdating weather information...\n");
    OpenWeatherMapCurrentData data;
    client.setLanguage(OPEN_WEATHER_MAP_LANGUAGE);
    client.setMetric(IS_METRIC);
    switch (lv_dropdown_get_selected(ui_LocationDropdown))
    {
    case 0:
    {
        lv_label_set_text(ui_locationtext, "Rackeve");
        client.updateCurrentById(&data, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATION_ID_RACKEVE);
        break;
    }
    case 1:
    {
        lv_label_set_text(ui_locationtext, "Budapest");
        client.updateCurrentById(&data, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATION_ID_BUDAPEST);
        break;
    }
    case 2:
    {
        lv_label_set_text(ui_locationtext, "Kiskunlachaza");
        client.updateCurrentById(&data, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATION_ID_KISKUNLACHAZA);
        break;
    }
    }
    if (data.main.equals("Clear"))
    {
        lv_obj_clear_flag(ui_clearimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_brokencloudsimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_snowimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_fogimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_lightrainimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_thunderstormimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_rainimg, LV_OBJ_FLAG_HIDDEN);
    }
    if (data.main.equals("Thunderstorm"))
    {
        lv_obj_clear_flag(ui_thunderstormimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_clearimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_brokencloudsimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_snowimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_fogimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_lightrainimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_rainimg, LV_OBJ_FLAG_HIDDEN);
    }
    if (data.main.equals("Drizzle"))
    {
        lv_obj_clear_flag(ui_lightrainimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_clearimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_brokencloudsimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_snowimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_fogimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_thunderstormimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_rainimg, LV_OBJ_FLAG_HIDDEN);
    }
    if (data.main.equals("Rain"))
    {
        lv_obj_clear_flag(ui_rainimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_clearimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_brokencloudsimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_snowimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_fogimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_lightrainimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_thunderstormimg, LV_OBJ_FLAG_HIDDEN);
    }
    if (data.main.equals("Snow"))
    {
        lv_obj_clear_flag(ui_snowimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_clearimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_brokencloudsimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_fogimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_lightrainimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_thunderstormimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_rainimg, LV_OBJ_FLAG_HIDDEN);
    }
    if (data.main.equals("Atmosphere"))
    {
        lv_obj_clear_flag(ui_brokencloudsimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_clearimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_snowimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_fogimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_lightrainimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_thunderstormimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_rainimg, LV_OBJ_FLAG_HIDDEN);
    }
    if (data.main.equals("Clouds"))
    {
        lv_obj_clear_flag(ui_brokencloudsimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_clearimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_snowimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_fogimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_lightrainimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_thunderstormimg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui_rainimg, LV_OBJ_FLAG_HIDDEN);
    }

    lv_label_set_text(ui_weatherinfo, StringToUpperCase(data.description).c_str()); // Set description
    char temp[5];                                                                   // Buffer big enough for 6-character float
    dtostrf(data.temp, 6, 1, temp);                                                 // Leave room for too large numbers!
    lv_label_set_text(ui_temptext, temp);                                           // Set temp
    char humid[5];                                                                  // Buffer big enough for 6-character float
    dtostrf(data.humidity, 6, 0, humid);                                            // Leave room for too large numbers!
    lv_label_set_text(ui_humidtext, humid);                                         // Set humidity
    if (data.description.equals(""))
    {
        SD_LOG("Updating failed! Requesting weather data again...\n");
        delay(200);
        InitWeather();
    }
    else
        SD_LOG("Updating successful!\n");
}