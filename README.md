## WT32-SC01 PLUS: Clock, Weather Station, and Online Radio Datastream Receiver.
### Features
|Feature|Image|
| --------- | --------- |
|**Clock and weather**|<img src="https://github.com/SubCoderHUN/WT32-SC01-PLUS/blob/main/github_images/mainscreen.png?raw=true" alt="" width="400" height="266" />|
|**Configure Wi-Fi, brightness, and location**|<img src="https://github.com/SubCoderHUN/WT32-SC01-PLUS/blob/main/github_images/optionsscreen.png?raw=true" alt="" width="400" height="266" />|
|**Online radio player**|<img src="https://github.com/SubCoderHUN/WT32-SC01-PLUS/blob/main/github_images/radioscreen.png?raw=true" alt="" width="400" height="266" />|
|**Saving WiFi credentials, screen brightness, and location to EEPROM**|<img src="https://github.com/SubCoderHUN/WT32-SC01-PLUS/blob/main/github_images/eeprommanager.png?raw=true" alt="" width="400" height="266" />|

### PlatformIO
This source was written under **[PlatformIO](https://platformio.org "PlatformIO")**. It's a great, user-friendly, and extensible integrated development environment. Highly recommended instead of the Arduino IDE.

### LVGL
For this project, I used the Light and Versatile Graphics Library (**[LVGL](https://github.com/lvgl/lvgl "LVGL")**) to create the interface.
You can find the project file for **[SquareLine Studio](https://squareline.io "SquareLine Studio")** (WT32-SC01-PLUS.spj) in the source code.

### Compile & GO
Everything is set up. You just need to fire up VSCode and compile the source code.
...and change the COM port in platformio.ini.😜

### Just in case...
If you messed up something and the ESP32 is stuck in a bootloop, you can erase the flash memory with `esptool.py --chip esp32-s3 erase_flash`.
Or, you can **[download esptool.exe from GitHub](https://github.com/espressif/esptool "download esptool.exe from GitHub")** and run the following command:  `esptool.exe --chip esp32-s3 erase_flash`.

### Plans for the future
1. In the future, I want to find some uses for the unused GPIO pins and SDCard.
2. I want to create an hourly forecast; at the moment, it only displays the current weather information.
3. .... and so on 😄
