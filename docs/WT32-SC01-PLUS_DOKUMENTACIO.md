# WT32-SC01 PLUS – Átfogó dokumentáció

## 1. Projektcél és fő funkciók
A WT32-SC01 PLUS egy ESP32-S3 alapú, érintőkijelzős asztali eszköz, amely órát, aktuális időjárási információkat és online rádió streamet jelenít meg, miközben a felhasználói beállításokat tartósan eltárolja és opcionálisan SD-kártyára naplóz.【F:README.md†L1-L30】 A projekt PlatformIO környezetre épül, a felület megjelenítését az LVGL és a LovyanGFX könyvtárak biztosítják, a grafikus felület forrása SquareLine Studio projektként is elérhető.【F:README.md†L33-L40】【F:README.md†L46-L49】

## 2. Könyvtárstruktúra áttekintése
- `WT32-SC01-PLUS/src/main.cpp`: a futási logika belépési pontja, amely inicializálja a kijelzőt, a hálózatot, a háttérfeladatokat és a funkcionális modulokat.【F:WT32-SC01-PLUS/src/main.cpp†L24-L471】
- `WT32-SC01-PLUS/src/Managers/…`: moduláris kezelők az idő (TimeManager), Wi-Fi (WiFiManager), EEPROM (EEPROMManager) és SD-kártya (SDManager) funkciókhoz.【F:WT32-SC01-PLUS/src/main.cpp†L16-L21】【F:WT32-SC01-PLUS/src/Managers/TimeManager/time_manager.cpp†L1-L73】【F:WT32-SC01-PLUS/src/Managers/WiFiManager/wifi_manager.cpp†L1-L191】【F:WT32-SC01-PLUS/src/Managers/EEPROMManager/eeprom_manager.cpp†L6-L120】【F:WT32-SC01-PLUS/src/Managers/SDManager/sd_manager.cpp†L1-L82】
- `WT32-SC01-PLUS/src/Features/…`: magasabb szintű szolgáltatások (időjárás, rádió) implementációi.【F:WT32-SC01-PLUS/src/main.cpp†L19-L21】【F:WT32-SC01-PLUS/src/Features/weather/weather.cpp†L1-L134】【F:WT32-SC01-PLUS/src/Features/radio/radio.cpp†L1-L143】
- `WT32-SC01-PLUS/src/ui.*` és `screens/`: a SquareLine Studio által generált LVGL felület definíciói.

## 3. Hardverkövetelmények és perifériák
- **Kijelző és érintés:** A projekt az LGFX absztrakciót használja a WT32-SC01 PLUS 480×320-as IPS panelének meghajtására. A párhuzamos 8 bites busz WR (47), RS (0) és D0–D7 (9, 46, 3, 8, 18, 17, 16, 15) pineken keresztül csatlakozik. A kijelző reset (4), háttérvilágítás PWM (45) és FT5x06 érintővezérlő I²C lábai (SDA 6, SCL 5) szintén konfigurálva vannak.【F:WT32-SC01-PLUS/src/main.cpp†L38-L116】
- **Audio (I2S):** Az online rádió a beépített ESP32-AudioI2S könyvtárral szólal meg, a BCLK (36), LRC (35) és DOUT (37) lábakon csatlakoztatott digitális erősítőn keresztül.【F:WT32-SC01-PLUS/src/Features/radio/radio.cpp†L6-L42】【F:WT32-SC01-PLUS/src/Features/radio/radio.cpp†L140-L143】
- **SD-kártya:** A microSD-kártya SPI üzemmódban érhető el, CS=41, CLK=39, CMD=40, D0=38. A `SDCARD_INSERTED` globálissal lehet jelezni, hogy rendelkezésre áll-e SD-kártya naplózáshoz.【F:WT32-SC01-PLUS/src/Managers/SDManager/sd_manager.h†L4-L12】【F:WT32-SC01-PLUS/src/main.cpp†L24-L33】【F:WT32-SC01-PLUS/src/Managers/SDManager/sd_manager.cpp†L5-L82】
- **Egyéb:** A projekt opcionálisan Wake-on-LAN csomagot küld a megadott MAC-címre a kijelzőn lévő Wi-Fi ikon megérintésére.【F:WT32-SC01-PLUS/src/main.cpp†L448-L454】【F:WT32-SC01-PLUS/src/Managers/WiFiManager/wifi_manager.cpp†L18-L25】

## 4. Fejlesztőkörnyezet és fordítás
A `platformio.ini` konfigurációban az `esp32-s3-devkitc-1` lapka beállításai, a szükséges könyvtárfüggőségek, valamint a PSRAM-használathoz kapcsolódó fordítási flagek találhatók. A soros monitor alapértelmezett sebessége 115200 baud.【F:WT32-SC01-PLUS/platformio.ini†L11-L40】

### Lépések a fordításhoz és feltöltéshez
1. Klónozd a repót és nyisd meg VS Code + PlatformIO IDE környezetben.
2. Ellenőrizd a `platformio.ini` fájlban az USB soros port (`monitor_port`) értékét, majd válaszd ki a megfelelő COM portot a feltöltéshez.【F:WT32-SC01-PLUS/platformio.ini†L19-L40】
3. Válaszd a `PlatformIO: Upload` parancsot, vagy futtasd terminálból: `pio run --target upload`.
4. A logokat a soros monitoron (`PlatformIO: Monitor`) követheted.

## 5. Indítási sorrend és fő futási ciklus
A `setup()` függvény inicializálja a kijelzőt, az LVGL drivereket, létrehozza a Wi-Fi felügyeleti feladatot, betölti az EEPROM-ban tárolt beállításokat, majd időzítőket állít be az időjárás frissítésére és a hálózat felügyeletére.【F:WT32-SC01-PLUS/src/main.cpp†L402-L444】 A `loop()` LVGL eseményeket kezel, felhasználói interakciókra reagál (Wake-on-LAN, Wi-Fi csatlakozás), és sorban hívja a fő szolgáltatásokat: idő frissítése, virtuális billentyűzet, fényerő, helyszín, időjárás frissítés, sötétítés és rádió stabilizálás.【F:WT32-SC01-PLUS/src/main.cpp†L446-L471】

## 6. Hálózati kapcsolódás és Wake-on-LAN
A WiFiManager modul kezeli a csatlakozást, a felhasználói bevitelt és a hibakezelést. Az `InitWifi()` felhasználói bevitelt vesz át LVGL textareákról, menti a hitelesítő adatokat EEPROM-ba, majd siker esetén frissíti az UI állapotát.【F:WT32-SC01-PLUS/src/Managers/WiFiManager/wifi_manager.cpp†L103-L157】 Az `StartWifiFromEEPROM()` automatikusan újracsatlakozik a korábban mentett hálózathoz, külön számlálóval kezelve a próbálkozásokat.【F:WT32-SC01-PLUS/src/Managers/WiFiManager/wifi_manager.cpp†L61-L102】 A `WiFiErrorHandling` FreeRTOS feladat 5 másodpercenként pingeli a Google-t; tartós hiba esetén újraindítja az ESP32-t a stabilitás érdekében.【F:WT32-SC01-PLUS/src/main.cpp†L430-L443】【F:WT32-SC01-PLUS/src/Managers/WiFiManager/wifi_manager.cpp†L36-L59】 A Wake-on-LAN funkcióval a kijelzőn lévő ikon aktiválása mágikus csomagot küld a megadott MAC-címre.【F:WT32-SC01-PLUS/src/main.cpp†L448-L454】【F:WT32-SC01-PLUS/src/Managers/WiFiManager/wifi_manager.cpp†L18-L25】

## 7. Időkezelés
A TimeManager modul NTP-szerverről (pool.ntp.org) szinkronizálja az időt, automatikusan kezeli a nyári-téli időszámítást, majd frissíti a felhasználói felülethez használt `hour`, `minute`, `date` stringeket.【F:WT32-SC01-PLUS/src/Managers/TimeManager/time_manager.h†L1-L9】【F:WT32-SC01-PLUS/src/Managers/TimeManager/time_manager.cpp†L7-L73】 A `DisplayTime()` minden ciklusban meghívja a formázott értékeket és frissíti az LVGL címkéket.【F:WT32-SC01-PLUS/src/main.cpp†L177-L183】

## 8. Időjárás funkció
Az `InitWeather()` OpenWeatherMap API-val dolgozik; a felhasználó által kiválasztott hely ID-je alapján letölti az aktuális adatokat, frissíti a hőmérsékletet, páratartalmat, leírást és az állapotnak megfelelő ikonokat. Hiba esetén rövid késleltetés után újrapróbálkozik, miközben az eredményeket SD-kártyára is logolja.【F:WT32-SC01-PLUS/src/Features/weather/weather.cpp†L6-L134】 A frissítések 5 percenként automatikusan megtörténnek a `SimpleTimer` időzítővel, illetve helyváltáskor azonnal új lekérdezés indul.【F:WT32-SC01-PLUS/src/main.cpp†L28-L29】【F:WT32-SC01-PLUS/src/main.cpp†L311-L335】【F:WT32-SC01-PLUS/src/main.cpp†L441-L468】

## 9. Online rádió lejátszó
A rádió modul előre konfigurált stream URL-listából dolgozik, az LVGL legördülő menüben kiválasztott csatornát indítja. A lejátszás állapota, a kiválasztott állomás és a hangerő EEPROM-ban kerül mentésre, így újraindítás után is visszaállnak a beállítások.【F:WT32-SC01-PLUS/src/Features/radio/radio.cpp†L14-L139】 A lejátszást külön FreeRTOS feladat végzi, amely az LVGL eseményeket figyeli és futtatja az `audio.loop()` ciklust a stream stabilitása érdekében.【F:WT32-SC01-PLUS/src/Features/radio/radio.cpp†L43-L95】

## 10. Képernyő fényerő és automatikus sötétítés
A felhasználó manuálisan állíthatja a fényerőt csúszkával; az érték azonnal alkalmazásra kerül és EEPROM-ba íródik.【F:WT32-SC01-PLUS/src/main.cpp†L203-L227】 Az automatikus sötétítéshez két időpont (kezdés és befejezés) választható `lv_roller` komponensekkel; a logika kezeli az éjszakába átnyúló intervallumokat és a visszaállítást is.【F:WT32-SC01-PLUS/src/main.cpp†L228-L309】

## 11. Helyválasztás és EEPROM-perzisztencia
A kiválasztott település, Wi-Fi hitelesítő adatok, fényerő és rádió beállítások EEPROM-ba kerülnek, a címkiosztást az `EEPROMManager` kommentje dokumentálja (SSID:0, jelszó:64, fényerő:100, stb.).【F:WT32-SC01-PLUS/src/Managers/EEPROMManager/eeprom_manager.cpp†L6-L118】 Az `RestoreFromEEPROM()` induláskor visszatölti ezeket az értékeket, és szükség esetén újraindítja az idő- és rádiómodulokat.【F:WT32-SC01-PLUS/src/main.cpp†L337-L399】

## 12. Naplózás SD-kártyára
Amennyiben az `SDCARD_INSERTED` igaz, a rendszer indításkor létrehozza a `log.txt` fájlt, majd a `SD_LOG` segédfüggvény minden kulcsfontosságú eseményt (Wi-Fi kapcsolódás, időjárás frissítés stb.) sorosan és az SD-kártyán is rögzít.【F:WT32-SC01-PLUS/src/main.cpp†L24-L25】【F:WT32-SC01-PLUS/src/Managers/SDManager/sd_manager.cpp†L43-L82】

## 13. Felhasználói felület és SquareLine Studio integráció
Az LVGL felületet a `ui_init()` inicializálja; a projekt forrása SquareLine Studio (`WT32-SC01-PLUS.spj`) formátumban is elérhető, így a grafikus elemek vizuálisan szerkeszthetők, majd a generált C források közvetlenül a `src` könyvtárba kerülnek.【F:README.md†L46-L49】【F:WT32-SC01-PLUS/src/main.cpp†L430-L444】

## 14. Hibakezelés és karbantartás
- **Wi-Fi megszakadás:** a háttérfeladat pingeléssel figyeli a kapcsolatot, hiba esetén újraindítja a vezérlőt a megbízhatóság érdekében.【F:WT32-SC01-PLUS/src/Managers/WiFiManager/wifi_manager.cpp†L36-L59】
- **Időjárás API:** üres válasz esetén azonnali újrapróbálkozás történik.【F:WT32-SC01-PLUS/src/Features/weather/weather.cpp†L126-L133】
- **Bootloop helyreállítás:** szükség esetén a README-ben dokumentált `esptool.py --chip esp32-s3 erase_flash` paranccsal törölhető a flash tartalma.【F:README.md†L52-L54】

## 15. Testreszabási lehetőségek
- **Új helyszínek:** egészítsd ki az OpenWeatherMap városazonosítókat és a legördülő menü opcióit a `weather.cpp` és az LVGL projekt módosításával.【F:WT32-SC01-PLUS/src/Features/weather/weather.cpp†L6-L47】
- **További rádióállomások:** bővítsd a `stations` és `stations_name` tömböket, ügyelve az EEPROM-ban tárolt indexekre.【F:WT32-SC01-PLUS/src/Features/radio/radio.cpp†L17-L34】
- **SD naplózás kikapcsolása:** állítsd `false` értékre a `SDCARD_INSERTED` globálist, így az SD-kezelő kód elkerüli a fájlműveleteket.【F:WT32-SC01-PLUS/src/main.cpp†L24-L33】
- **Nyelvi lokalizáció:** a SquareLine Studio projektben szerkeszthetők a feliratok; a `weather.cpp` `OPEN_WEATHER_MAP_LANGUAGE` változójának módosításával más nyelvű API válasz kérhető.【F:WT32-SC01-PLUS/src/Features/weather/weather.cpp†L6-L27】

## 16. Jövőbeli ötletek
A README javaslatokat is tartalmaz a további fejlesztésekre (szabad GPIO-k kihasználása, óránkénti előrejelzés), amelyek jó kiindulási alapot jelenthetnek új funkciók tervezéséhez.【F:README.md†L57-L59】

---
Ez a dokumentum a projekt forráskódjának jelenlegi állapotára épül, és referenciaként szolgál a hardveres bekötéshez, a szoftver architektúrájához, valamint a testreszabási és karbantartási feladatokhoz.
