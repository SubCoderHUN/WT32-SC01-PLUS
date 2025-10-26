# WT32-SC01 PLUS Smart Desk Companion

The WT32-SC01 PLUS project turns Espressif's ESP32-S3 based touch display module into a network-connected desk companion. It combines a clock, live weather information from OpenWeatherMap, an online radio player, and a configurable interface that stores preferences in EEPROM and can optionally log activity to an SD card.

This repository contains everything needed to build, flash, and customize the firmware, together with the SquareLine Studio project used to design the LVGL user interface.

## Features

| Feature | Screenshot |
| --- | --- |
| Clock and current weather | <img src="https://github.com/SubCoderHUN/WT32-SC01-PLUS/blob/main/github_images/mainscreen.png?raw=true" alt="Clock and weather screen" width="400" height="266" /> |
| Configure Wi-Fi, brightness, and location | <img src="https://github.com/SubCoderHUN/WT32-SC01-PLUS/blob/main/github_images/optionsscreen.png?raw=true" alt="Settings screen" width="400" height="266" /> |
| Manual and scheduled brightness control | <img src="https://github.com/SubCoderHUN/WT32-SC01-PLUS/blob/main/github_images/brightnessscreen.png?raw=true" alt="Brightness control screen" width="400" height="266" /> |
| Online radio playback | <img src="https://github.com/SubCoderHUN/WT32-SC01-PLUS/blob/main/github_images/radioscreen.png?raw=true" alt="Radio screen" width="400" height="266" /> |
| EEPROM persistence for preferences | <img src="https://github.com/SubCoderHUN/WT32-SC01-PLUS/blob/main/github_images/eeprommanager.png?raw=true" alt="EEPROM manager" width="400" height="266" /> |
| Optional SD card logging | <img src="https://github.com/SubCoderHUN/WT32-SC01-PLUS/blob/main/github_images/sd_manager.png?raw=true" alt="SD logging" width="400" height="266" /> |

## Hardware requirements

- WT32-SC01 PLUS (ESP32-S3 with 480×320 capacitive touch display)
- Optional I2S audio amplifier and speaker connected to BCLK (GPIO 36), LRC (GPIO 35), and DOUT (GPIO 37)
- Optional microSD card wired for SPI (CS 41, CLK 39, CMD 40, D0 38)
- USB-C cable capable of data transfer for flashing

## Software prerequisites

Before building the firmware ensure that:

1. [Visual Studio Code](https://code.visualstudio.com/) is installed.
2. The [PlatformIO IDE extension](https://platformio.org/install/ide?install=vscode) is installed in VS Code.
3. USB drivers for the WT32-SC01 PLUS are available on your operating system.

## Quick start

1. Clone this repository and open the folder in VS Code.
2. When prompted, allow PlatformIO to install the required ESP32 toolchains and libraries.
3. Connect the WT32-SC01 PLUS over USB and note the serial port name (COMx on Windows, `/dev/ttyUSBx` on Linux, `/dev/cu.usbmodem*` on macOS).
4. Open `WT32-SC01-PLUS/platformio.ini` and adjust the `monitor_port` if PlatformIO cannot auto-detect your device.
5. Build and upload the firmware using the **PlatformIO: Upload** command or run `pio run --target upload` in the integrated terminal.
6. Launch **PlatformIO: Monitor** to observe boot logs and confirm Wi-Fi connections, weather updates, or radio stream status.

## Configuring the device

After flashing, the on-device UI guides you through configuring Wi-Fi credentials, choosing a city for weather data, picking an online radio station, and setting brightness. All user selections are written to EEPROM so they persist between reboots. If an SD card is present and `SDCARD_INSERTED` is set to `true` in the source, logs are stored in `log.txt` on the card.

## Customizing the interface

The UI is built with [LVGL](https://lvgl.io/) and rendered via [LovyanGFX](https://www.arduino.cc/reference/en/libraries/lovyangfx/). You can edit the visuals in [SquareLine Studio](https://squareline.io/) using the provided `WT32-SC01-PLUS.spj` project file. After exporting the updated LVGL sources, replace the generated files under `WT32-SC01-PLUS/src` to apply the changes.

## Troubleshooting

- **Board stuck in boot loop:** erase the flash with `esptool.py --chip esp32-s3 erase_flash` (Python) or `esptool.exe --chip esp32-s3 erase_flash` (Windows executable) before reflashing.
- **No audio output:** confirm the I2S pins match the defaults above and the amplifier is powered.
- **Weather updates fail:** verify the device has internet access and that the OpenWeatherMap API key in `weather.cpp` is valid.
- **SD logging disabled:** set `SDCARD_INSERTED` to `true` in `src/main.cpp` only if a microSD card is wired; otherwise leave it `false` to skip SD initialization.

## Future enhancements

Planned improvements include using the remaining GPIOs for expansion modules and adding hourly weather forecasts alongside the current conditions.

## Further reading

- [English technical documentation](docs/WT32-SC01-PLUS_DOCUMENTATION_EN.md)
- [Hungarian technical documentation](docs/WT32-SC01-PLUS_DOKUMENTACIO.md)

