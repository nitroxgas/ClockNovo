#include "Globals.h"
#include "Preferences.h"
#include <WiFi.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

Preferences Settings;

char *getID()
{
    uint8_t mac[6];
    WiFi.macAddress(mac);
    char *macStr = new char[24];
    snprintf(macStr, 24, "clock_%02x%02x%02x", mac[3], mac[4], mac[5]);
    DEBUG_PRINTLN(F("Starting filesystem"));
    return macStr;
}

void startLittleFS()
{
    DEBUG_PRINTLN(F("Starting filesystem"));
    if (LittleFS.begin())
    {
#ifdef ULANZI
        LittleFS.mkdir("/MELODIES");
#endif
        LittleFS.mkdir("/ICONS");
        DEBUG_PRINTLN(F("Filesystem started"));
    }
    else
    {
        DEBUG_PRINTLN(F("Filesystem currupt. Formating..."));
        LittleFS.format();
        ESP.restart();
    }
}

void loadDevSettings()
{
    DEBUG_PRINTLN("Loading Devsettings");
    if (LittleFS.exists("/dev.json"))
    {
        File file = LittleFS.open("/dev.json", "r");
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, file);
        if (error)
        {
            DEBUG_PRINTLN(F("Failed to read dev settings"));
            return;
        }

        DEBUG_PRINTF("%i dev settings found", doc.size());

        if (doc.containsKey("bootsound"))
        {
            BOOT_SOUND = doc["bootsound"].as<String>();
        }

        if (doc.containsKey("sensor_reading"))
        {
            SENSOR_READING = doc["sensor_reading"].as<bool>();
        }

        if (doc.containsKey("matrix"))
        {
            MATRIX_LAYOUT = doc["matrix"];
        }

        if (doc.containsKey("temp_offset"))
        {
            TEMP_OFFSET = doc["temp_offset"];
        }

        if (doc.containsKey("min_brightness"))
        {
            MIN_BRIGHTNESS = doc["min_brightness"];
        }

        if (doc.containsKey("max_brightness"))
        {
            MAX_BRIGHTNESS = doc["max_brightness"];
        }

        if (doc.containsKey("hum_offset"))
        {
            HUM_OFFSET = doc["hum_offset"];
        }

        if (doc.containsKey("uppercase"))
        {
            UPPERCASE_LETTERS = doc["uppercase"].as<bool>();
        }

        if (doc.containsKey("update_check"))
        {
            UPDATE_CHECK = doc["update_check"].as<bool>();
        }

        if (doc.containsKey("temp_dec_places"))
        {
            TEMP_DECIMAL_PLACES = doc["temp_dec_places"].as<int>();
        }

        if (doc.containsKey("rotate_screen"))
        {
            ROTATE_SCREEN = doc["rotate_screen"].as<bool>();
        }

        if (doc.containsKey("gamma"))
        {
            GAMMA = doc["gamma"].as<float>();
        }

        if (doc.containsKey("color_correction"))
        {
            auto correction = doc["color_correction"];
            if (correction.is<JsonArray>() && correction.size() == 3)
            {
                uint8_t r = correction[0];
                uint8_t g = correction[1];
                uint8_t b = correction[2];
               // COLOR_CORRECTION.setRGB(r, g, b);
            }
        }

        if (doc.containsKey("color_temperature"))
        {
            auto temperature = doc["color_temperature"];
            if (temperature.is<JsonArray>() && temperature.size() == 3)
            {
                uint8_t r = temperature[0];
                uint8_t g = temperature[1];
                uint8_t b = temperature[2];
                // COLOR_TEMPERATURE.setRGB(r, g, b);
            }
        }

        file.close();
    }
    else
    {
        DEBUG_PRINTLN("Devsettings not found");
    }
}

void loadSettings()
{
    startLittleFS();
    DEBUG_PRINTLN(F("Loading Usersettings"));
    Settings.begin("clock", false);
    BRIGHTNESS = Settings.getUInt("BRI", 120);
    AUTO_BRIGHTNESS = Settings.getBool("ABRI", false);
    TEXTCOLOR_565 = Settings.getUInt("TCOL", 0xFFFF);

    TIME_COLOR = Settings.getUInt("TIME_COL", 0);
    DATE_COLOR = Settings.getUInt("DATE_COL", 0);
    TEMP_COLOR = Settings.getUInt("TEMP_COL", 0);
    HUM_COLOR = Settings.getUInt("HUM_COL", 0);
#ifdef ULANZI
    BAT_COLOR = Settings.getUInt("BAT_COL", 0);
#endif
    WDC_ACTIVE = Settings.getUInt("WDCA", 0xFFFF);
    WDC_INACTIVE = Settings.getUInt("WDCI", 0x6B6D);
    AUTO_TRANSITION = Settings.getBool("ATRANS", true);
    SHOW_WEEKDAY = Settings.getBool("WD", true);
    TIME_PER_TRANSITION = Settings.getUInt("TSPEED", 400);
    TIME_PER_APP = Settings.getUInt("ATIME", 7000);
    TIME_FORMAT = Settings.getString("TFORMAT", "%H:%M:%S");
    DATE_FORMAT = Settings.getString("DFORMAT", "%d.%m.%y");
    START_ON_MONDAY = Settings.getBool("SOM", true);
    IS_CELSIUS = Settings.getBool("CEL", true);
    SHOW_TIME = Settings.getBool("TIM", true);
    SHOW_EYES = Settings.getBool("EYE", true);
    SHOW_DATE = Settings.getBool("DAT", true);
    SHOW_TEMP = Settings.getBool("TEMP", true);
    SHOW_HUM = Settings.getBool("HUM", true);
    MATRIX_LAYOUT = Settings.getUInt("MAT", 0);
    SCROLL_SPEED = Settings.getUInt("SSPEED", 100);
#ifdef ULANZI
    SHOW_BAT = Settings.getBool("BAT", true);
#endif
    SOUND_ACTIVE = Settings.getBool("SOUND", true);
#ifndef ULANZI
    VOLUME_PERCENT = Settings.getUInt("VOL", 50);
    VOLUME = map(VOLUME_PERCENT, 0, 100, 0, 30);
#endif
    Settings.end();
    uniqueID = getID();
    MQTT_PREFIX = String(uniqueID);
    loadDevSettings();
}

void saveSettings()
{
    DEBUG_PRINTLN(F("Saving usersettings"));
    Settings.begin("clock", false);
    Settings.putUInt("BRI", BRIGHTNESS);
    Settings.putBool("WD", SHOW_WEEKDAY);
    Settings.putBool("ABRI", AUTO_BRIGHTNESS);
    Settings.putBool("ATRANS", AUTO_TRANSITION);
    Settings.putUInt("TCOL", TEXTCOLOR_565);

    Settings.putUInt("TIME_COL", TIME_COLOR);
    Settings.putUInt("DATE_COL", DATE_COLOR);
    Settings.putUInt("TEMP_COL", TEMP_COLOR);
    Settings.putUInt("HUM_COL", HUM_COLOR);
#ifdef ULANZI
    Settings.putUInt("BAT_COL", BAT_COLOR);
#endif

    Settings.putUInt("WDCA", WDC_ACTIVE);
    Settings.putUInt("WDCI", WDC_INACTIVE);
    Settings.putUInt("TSPEED", TIME_PER_TRANSITION);
    Settings.putUInt("ATIME", TIME_PER_APP);
    Settings.putString("TFORMAT", TIME_FORMAT);
    Settings.putString("DFORMAT", DATE_FORMAT);
    Settings.putBool("SOM", START_ON_MONDAY);
    Settings.putBool("CEL", IS_CELSIUS);
    Settings.putBool("TIM", SHOW_TIME);
    Settings.putBool("EYE", SHOW_EYES);
    Settings.putBool("DAT", SHOW_DATE);
    Settings.putBool("TEMP", SHOW_TEMP);
    Settings.putBool("HUM", SHOW_HUM);
    Settings.putUInt("SSPEED", SCROLL_SPEED);
#ifdef ULANZI
    Settings.putBool("BAT", SHOW_BAT);
#endif
    Settings.putBool("SOUND", SOUND_ACTIVE);
#ifndef ULANZI
    Settings.putUInt("VOL", VOLUME_PERCENT);
#endif
    Settings.end();
}

const char *uniqueID;
IPAddress local_IP;
IPAddress gateway;
IPAddress subnet;
IPAddress primaryDNS;
IPAddress secondaryDNS;
const char *VERSION = "0.70";

String MQTT_HOST = "";
uint16_t MQTT_PORT = 1883;
String MQTT_USER;
String MQTT_PASS;
String MQTT_PREFIX;
bool IO_BROKER = false;
bool NET_STATIC = false;
bool SHOW_TIME = true;
bool SHOW_EYES = true;
bool SHOW_DATE = true;
bool SHOW_WEATHER = true;
#ifdef ULANZI
bool SHOW_BAT = true;
#endif
bool SHOW_TEMP = true;
bool SHOW_HUM = true;
bool SHOW_SECONDS = true;
bool SHOW_WEEKDAY = true;
String NET_IP = "192.168.178.10";
String NET_GW = "192.168.178.1";
String NET_SN = "255.255.255.0";
String NET_PDNS = "8.8.8.8";
String NET_SDNS = "1.1.1.1";
long TIME_PER_APP = 7000;
uint8_t MATRIX_FPS = 50;
int TIME_PER_TRANSITION = 400;
String NTP_SERVER = "de.pool.ntp.org";
String NTP_TZ = "CET-1CEST,M3.5.0,M10.5.0/3";
bool HA_DISCOVERY = false;

// Periphery
String CURRENT_APP;
float CURRENT_TEMP;
bool IS_CELSIUS;
#ifndef ULANZI
uint8_t TEMP_SENSOR_TYPE = TEMP_SENSOR_TYPE_NONE;
#endif
float CURRENT_HUM;
float CURRENT_LUX;
int BRIGHTNESS = 120;
int BRIGHTNESS_PERCENT;

#ifdef ULANZI
float TEMP_OFFSET = -9;
uint8_t BATTERY_PERCENT;
uint16_t BATTERY_RAW;
#else
float TEMP_OFFSET;
#endif
float HUM_OFFSET;
uint16_t LDR_RAW;
String TIME_FORMAT = "%H:%M:%S";
String DATE_FORMAT = "%d.%m.%y";
bool START_ON_MONDAY;

String ALARM_SOUND;
uint8_t SNOOZE_TIME;

String TIMER_SOUND;

// Matrix States
bool AUTO_TRANSITION = false;
bool AUTO_BRIGHTNESS = true;
bool UPPERCASE_LETTERS = true;
bool AP_MODE;
bool MATRIX_OFF;
bool TIMER_ACTIVE;
bool ALARM_ACTIVE;
uint16_t TEXTCOLOR_565;
bool SOUND_ACTIVE;
String BOOT_SOUND = "";
int TEMP_DECIMAL_PLACES = 0;
#ifndef ULANZI
uint8_t VOLUME_PERCENT;
uint8_t VOLUME;
#endif
int MATRIX_LAYOUT = 0;
bool UPDATE_AVAILABLE = false;
long RECEIVED_MESSAGES;
/* CRGB COLOR_CORRECTION;
CRGB COLOR_TEMPERATURE; */
uint16_t WDC_ACTIVE;
uint16_t WDC_INACTIVE;
bool BLOCK_NAVIGATION = false;
bool UPDATE_CHECK = false;
float GAMMA = 0;
bool SENSOR_READING = true;
bool ROTATE_SCREEN = false;
uint8_t SCROLL_SPEED = 100;
uint16_t TIME_COLOR = 0;
uint16_t DATE_COLOR = 0;
uint16_t BAT_COLOR = 0;
uint16_t TEMP_COLOR = 0;
uint16_t HUM_COLOR = 0;
bool ARTNET_MODE;
bool MOODLIGHT_MODE;
uint8_t MIN_BRIGHTNESS = 2;
uint8_t MAX_BRIGHTNESS = 180;
float movementFactor = 0.5;