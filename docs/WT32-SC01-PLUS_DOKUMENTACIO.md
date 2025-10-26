# WT32-SC01 PLUS – Átfogó dokumentáció

## 1. Projektcél és fő funkciók
A WT32-SC01 PLUS egy ESP32-S3 alapú, érintőkijelzős asztali eszköz, amely órát, aktuális időjárási információkat és online rádió streamet jelenít meg, miközben a felhasználói beállításokat tartósan eltárolja és opcionálisan SD-kártyára naplóz. A projekt PlatformIO környezetre épül, a felület megjelenítését az LVGL és a LovyanGFX könyvtárak biztosítják, a grafikus felület forrása SquareLine Studio projektként is elérhető.

## 2. Könyvtárstruktúra áttekintése
- `WT32-SC01-PLUS/src/main.cpp`: a futási logika belépési pontja, amely inicializálja a kijelzőt, a hálózatot, a háttérfeladatokat és a funkcionális modulokat
- `WT32-SC01-PLUS/src/Managers/…`: moduláris kezelők az idő (TimeManager), Wi-Fi (WiFiManager), EEPROM (EEPROMManager) és SD-kártya (SDManager) funkciókhoz.
- `WT32-SC01-PLUS/src/Features/…`: magasabb szintű szolgáltatások (időjárás, rádió) implementációi.
- `WT32-SC01-PLUS/src/ui.*` és `screens/`: a SquareLine Studio által generált LVGL felület definíciói.

## 3. Hardverkövetelmények és perifériák
- **Kijelző és érintés:** A projekt az LGFX absztrakciót használja a WT32-SC01 PLUS 480×320-as IPS panelének meghajtására. A párhuzamos 8 bites busz WR (47), RS (0) és D0–D7 (9, 46, 3, 8, 18, 17, 16, 15) pineken keresztül csatlakozik. A kijelző reset (4), háttérvilágítás PWM (45) és FT5x06 érintővezérlő I²C lábai (SDA 6, SCL 5) szintén konfigurálva vannak.
- **Audio (I2S):** Az online rádió a beépített ESP32-AudioI2S könyvtárral szólal meg, a BCLK (36), LRC (35) és DOUT (37) lábakon csatlakoztatott digitális erősítőn keresztül.
- **SD-kártya:** A microSD-kártya SPI üzemmódban érhető el, CS=41, CLK=39, CMD=40, D0=38. A `SDCARD_INSERTED` globálissal lehet jelezni, hogy rendelkezésre áll-e SD-kártya naplózáshoz.
- **Egyéb:** A projekt opcionálisan Wake-on-LAN csomagot küld a megadott MAC-címre a kijelzőn lévő Wi-Fi ikon megérintésére.

## 4. Fejlesztőkörnyezet és fordítás
A `platformio.ini` konfigurációban az `esp32-s3-devkitc-1` lapka beállításai, a szükséges könyvtárfüggőségek, valamint a PSRAM-használathoz kapcsolódó fordítási flagek találhatók. A soros monitor alapértelmezett sebessége 115200 baud.

### Lépések a fordításhoz és feltöltéshez
1. Klónozd a repót és nyisd meg VS Code + PlatformIO IDE környezetben.
2. Ellenőrizd a `platformio.ini` fájlban az USB soros port (`monitor_port`) értékét, majd válaszd ki a megfelelő COM portot a feltöltéshez.
3. Válaszd a `PlatformIO: Upload` parancsot, vagy futtasd terminálból: `pio run --target upload`.
4. A logokat a soros monitoron (`PlatformIO: Monitor`) követheted.

## 5. Indítási sorrend és fő futási ciklus
A `setup()` függvény inicializálja a kijelzőt, az LVGL drivereket, létrehozza a Wi-Fi felügyeleti feladatot, betölti az EEPROM-ban tárolt beállításokat, majd időzítőket állít be az időjárás frissítésére és a hálózat felügyeletére. A `loop()` LVGL eseményeket kezel, felhasználói interakciókra reagál (Wake-on-LAN, Wi-Fi csatlakozás), és sorban hívja a fő szolgáltatásokat: idő frissítése, virtuális billentyűzet, fényerő, helyszín, időjárás frissítés, sötétítés és rádió stabilizálás.

## 6. Hálózati kapcsolódás és Wake-on-LAN
A WiFiManager modul kezeli a csatlakozást, a felhasználói bevitelt és a hibakezelést. Az `InitWifi()` felhasználói bevitelt vesz át LVGL textareákról, menti a hitelesítő adatokat EEPROM-ba, majd siker esetén frissíti az UI állapotát. Az `StartWifiFromEEPROM()` automatikusan újracsatlakozik a korábban mentett hálózathoz, külön számlálóval kezelve a próbálkozásokat. A `WiFiErrorHandling` FreeRTOS feladat 5 másodpercenként pingeli a Google-t; tartós hiba esetén újraindítja az ESP32-t a stabilitás érdekében. A Wake-on-LAN funkcióval a kijelzőn lévő ikon aktiválása mágikus csomagot küld a megadott MAC-címre.

## 7. Időkezelés
A TimeManager modul NTP-szerverről (pool.ntp.org) szinkronizálja az időt, automatikusan kezeli a nyári-téli időszámítást, majd frissíti a felhasználói felülethez használt `hour`, `minute`, `date` stringeket. A `DisplayTime()` minden ciklusban meghívja a formázott értékeket és frissíti az LVGL címkéket.

## 8. Időjárás funkció
Az `InitWeather()` OpenWeatherMap API-val dolgozik; a felhasználó által kiválasztott hely ID-je alapján letölti az aktuális adatokat, frissíti a hőmérsékletet, páratartalmat, leírást és az állapotnak megfelelő ikonokat. Hiba esetén rövid késleltetés után újrapróbálkozik, miközben az eredményeket SD-kártyára is logolja. A frissítések 5 percenként automatikusan megtörténnek a `SimpleTimer` időzítővel, illetve helyváltáskor azonnal új lekérdezés indul.

## 9. Online rádió lejátszó
A rádió modul előre konfigurált stream URL-listából dolgozik, az LVGL legördülő menüben kiválasztott csatornát indítja. A lejátszás állapota, a kiválasztott állomás és a hangerő EEPROM-ban kerül mentésre, így újraindítás után is visszaállnak a beállítások. A lejátszást külön FreeRTOS feladat végzi, amely az LVGL eseményeket figyeli és futtatja az `audio.loop()` ciklust a stream stabilitása érdekében.

## 10. Képernyő fényerő és automatikus sötétítés
A felhasználó manuálisan állíthatja a fényerőt csúszkával; az érték azonnal alkalmazásra kerül és EEPROM-ba íródik. Az automatikus sötétítéshez két időpont (kezdés és befejezés) választható `lv_roller` komponensekkel; a logika kezeli az éjszakába átnyúló intervallumokat és a visszaállítást is.

## 11. Helyválasztás és EEPROM-perzisztencia
A kiválasztott település, Wi-Fi hitelesítő adatok, fényerő és rádió beállítások EEPROM-ba kerülnek, a címkiosztást az `EEPROMManager` kommentje dokumentálja (SSID:0, jelszó:64, fényerő:100, stb.). Az `RestoreFromEEPROM()` induláskor visszatölti ezeket az értékeket, és szükség esetén újraindítja az idő- és rádiómodulokat.

## 12. Naplózás SD-kártyára
Amennyiben az `SDCARD_INSERTED` igaz, a rendszer indításkor létrehozza a `log.txt` fájlt, majd a `SD_LOG` segédfüggvény minden kulcsfontosságú eseményt (Wi-Fi kapcsolódás, időjárás frissítés stb.) sorosan és az SD-kártyán is rögzít.

## 13. Felhasználói felület és SquareLine Studio integráció
Az LVGL felületet a `ui_init()` inicializálja; a projekt forrása SquareLine Studio (`WT32-SC01-PLUS.spj`) formátumban is elérhető, így a grafikus elemek vizuálisan szerkeszthetők, majd a generált C források közvetlenül a `src` könyvtárba kerülnek.

## 14. Hibakezelés és karbantartás
- **Wi-Fi megszakadás:** a háttérfeladat pingeléssel figyeli a kapcsolatot, hiba esetén újraindítja a vezérlőt a megbízhatóság érdekében.
- **Időjárás API:** üres válasz esetén azonnali újrapróbálkozás történik.
- **Bootloop helyreállítás:** szükség esetén a README-ben dokumentált `esptool.py --chip esp32-s3 erase_flash` paranccsal törölhető a flash tartalma.

## 15. Testreszabási lehetőségek
- **Új helyszínek:** egészítsd ki az OpenWeatherMap városazonosítókat és a legördülő menü opcióit a `weather.cpp` és az LVGL projekt módosításával.
- **További rádióállomások:** bővítsd a `stations` és `stations_name` tömböket, ügyelve az EEPROM-ban tárolt indexekre.
- **SD naplózás kikapcsolása:** állítsd `false` értékre a `SDCARD_INSERTED` globálist, így az SD-kezelő kód elkerüli a fájlműveleteket.
- **Nyelvi lokalizáció:** a SquareLine Studio projektben szerkeszthetők a feliratok; a `weather.cpp` `OPEN_WEATHER_MAP_LANGUAGE` változójának módosításával más nyelvű API válasz kérhető.

## 16. Jövőbeli ötletek
A README javaslatokat is tartalmaz a további fejlesztésekre (szabad GPIO-k kihasználása, óránkénti előrejelzés), amelyek jó kiindulási alapot jelenthetnek új funkciók tervezéséhez.

---
Ez a dokumentum a projekt forráskódjának jelenlegi állapotára épül, és referenciaként szolgál a hardveres bekötéshez, a szoftver architektúrájához, valamint a testreszabási és karbantartási feladatokhoz.
