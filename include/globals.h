#ifndef GLOBALS_H
#define GLOBALS_H

#pragma once

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <FastLED.h>
#include <ESPAsyncE131.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>

#define DEVICE_ID "LED_Cloud_5"                                                        
#define OTA_PORT 8005

#ifndef APSSID
#define APSSID DEVICE_ID
#define APPSK  "$p4nkE101"
#endif

#define NUM_LEDS 160                                                                       //Change for # LEDs in LED strip.
#define DATA_PIN 13
#define CLOCK_PIN 14
#define LED_TYPE WS2812B                                                                   //WS2812B or WS2801.
#define COLOR_ORDER GRB                                                                    //GRB for WS2812B, RBG for WS2801.
extern CRGB RGBLED[NUM_LEDS];

//Network Parameters
extern IPAddress gateway;
extern IPAddress subnet;
extern ESP8266WebServer server;
extern WiFiClientSecure client;
extern WiFiClient espClient;
extern WiFiServer tcpServer;
extern PubSubClient mqttClient;

//Mode/sACN/PurpleAir Parameters
extern uint8_t mode;
extern uint16_t universe;
extern uint16_t universeCount;
extern uint32_t sensor;
extern ESPAsyncE131 e131;

//HTTP Parameters
extern String purpleAirAPIurl;                                                             //PurpleAir base url.
extern String sensorNumber;                                                                //PurpleAir sensor number. (Default: 70169)
extern String apiKeyQuery;                                                                 //API Key query.
extern String apiKey;                                                                      //PurpleAir API key. (Default: 3D0F0CEE-32BF-11ED-B5AA-42010A800006)
extern String host;                                                                        //Web Server address to read/write from. "59921" is the Mills College sensor.
extern String newApiKey;
extern uint8_t apiKeyLength;
extern uint16_t pm25;                                                                           

// MQTT Broker settings
extern const char* mqtt_server;
extern const char* mqtt_username;
extern const char* mqtt_password;
extern const char* mqtt_topic_rx_all;
extern const char* mqtt_topic_rx_single;
extern const char* mqtt_topic_tx;
extern const int mqtt_port;

//Color HSV Parameters
extern uint8_t ledVal[], ledOn[], ledHue[], ledWhite[];
extern uint8_t currentHue;
extern uint8_t brightnessOn;
extern uint16_t numFadeLeds;                                                                //Maximum number of LEDs on at any given time while in Purple Air mode.
const uint8_t red = 0;
const uint8_t orange = 19;
const uint8_t yellow = 55;
const uint8_t green = 86;
const uint8_t blue = 120;
const uint8_t indigo = 160;
const uint8_t purple = 200;
const uint8_t pink = 250;
extern bool rainbowEn;
extern bool whiteEn;

//Time Parameters
extern uint32_t timeMilliseconds, ledPastTime, httpPastTime, resetTime;
extern bool resetEnable;

//HTML Parameters
const char refresh_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
  <head>
    <meta http-equiv="refresh" content="0; url='/'" />
  </head>
  <body>
  </body>
</html>
)rawliteral";

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
  <head>
    <title>LED Cloud Web UI</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
  </head>
  <body style="background-color:#777777">
    <h1>LED Cloud Web UI</h1>
    <h3><b>Set WiFi Network</b></h3>
    <form action="/get">
      <input type="text" name="SSID" size="10" maxlength="100"> SSID<br>
      <input type="text" name="Password" size="10" maxlength="100"> Password<br><br>
      <b>Static IP (leave blank for DHCP)</b><br>
      <input type="number" name="ip_1" min="0" max="255" size="3"> . <input type="number" name="ip_2" min="0" max="255" size="3"> . <input type="number" name="ip_3" min="0" max="255" size="3"> . <input type="number" name="ip_4" min="0" max="255" size="3"> IP<br>
      <input type="number" name="subnet_1" min="0" max="255" size="3"> . <input type="number" name="subnet_2" min="0" max="255" size="3"> . <input type="number" name="subnet_3" min="0" max="255" size="3"> . <input type="number" name="subnet_4" min="0" max="255" size="3"> Subnet<br>
      <input type="number" name="gateway_1" min="0" max="255" size="3"> . <input type="number" name="gateway_2" min="0" max="255" size="3"> . <input type="number" name="gateway_3" min="0" max="255" size="3"> . <input type="number" name="gateway_4" min="0" max="255" size="3"> Gateway<br>
      <input type="number" name="dns_1" min="0" max="255" size="3"> . <input type="number" name="dns_2" min="0" max="255" size="3"> . <input type="number" name="dns_3" min="0" max="255" size="3"> . <input type="number" name="dns_4" min="0" max="255" size="3"> DNS<br><br>  
      <input type="submit" value="Submit" onclick="alert('Network settings changed. Please restart to join new network.')"><br>
    </form>
    <h4>______________________________</h4>
    <h3><b>Set Mode</b></h3>
    <form action="/mode">
      <input type="radio" name="mode" id="mode" value="0"><label for="mode" value="0">sACN</label><br>
      <input type="radio" name="mode" id="mode" value="1"><label for="mode" value="1">PurpleAir AQI</label><br>
      <input type="radio" name="mode" id="mode" value="2"><label for="mode" value="2">Color Selector</label><br><br>
      <input type="number" name="universe" min="1" max="63999" size="5" value="1"> sACN Universe # (1 - 63999)<br>
      <input type="number" name="sensor_id" min="0" max="999999" size="5" value="0"> PurpleAir Sensor # (0 - 999999)<br>
      <input type="text" name="api_key" size="10" maxlength="36"> PurpleAir API Key<br>
      <a href="/sensor">(Where do I find the sensor number?)</a><br><br>
      <input type="submit" value="Submit" onclick="alert('Mode settings changed. Please restart for settings to take effect.')"><br><br>
    </form>
    <b>Color Selector Color</b><br>
    <form action="/color">
      <input type="radio" name="color" id="color" value="0"><label for="color" value="0">Red</label>
      <input type="radio" name="color" id="color" value="1"><label for="color" value="1">Orange</label>
      <input type="radio" name="color" id="color" value="2"><label for="color" value="2">Yellow</label>
      <input type="radio" name="color" id="color" value="3"><label for="color" value="3">Green</label><br>
      <input type="radio" name="color" id="color" value="4"><label for="color" value="4">Blue</label>
      <input type="radio" name="color" id="color" value="5"><label for="color" value="5">Indigo</label>
      <input type="radio" name="color" id="color" value="6"><label for="color" value="6">Purple</label>
      <input type="radio" name="color" id="color" value="7"><label for="color" value="7">Pink</label><br>
      <input type="radio" name="color" id="color" value="8"><label for="color" value="8">Rainbow</label>
      <input type="radio" name="color" id="color" value="9"><label for="color" value="9">White</label>
      <input type="radio" name="color" id="color" value="10"><label for="color" value="10">Off</label><br><br>
      <input type="submit" value="Set" onclick="alert('Color changed.')"><br>
    </form>
    <h4>______________________________</h4>
    <h3><b>Utility</b></h3>
    <form action="/clear">
      <input type="submit" value="Clear Settings" onclick="alert('Settings cleared. Please restart to enable access point.')">
    </form><br>
    <form action="/restart">
      <input type="submit" value="Restart">
    </form><br>
  </body>
</html>
)rawliteral";

const char purpleAir_info[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
  <h1><b>PurpleAir Sensor Number</b></h1>
  <p>
    <body style="background-color:#777777">
      1. Go to the <a href="https://map.purpleair.com/">PurpleAir Map</a>.<br>
      2. Select the sensor that you want to monitor with the LED Cloud.<br>
      3. Once selected, look at the URL in your web browser.<br>
      The 4 to 6-digit number in-between "select=" and the "#" is the number of the sensor.<br><br>
      Example URL:<br>
      https://map.purpleair.com/1/i/ls/mAQI/a10/p604800/cC0?select=<b style="color:#FFFFFF">84923</b>#15.62/37.798778/-122.197967<br><br>
      Purple Air Sensor Number: 84923</h2><br>
      <form action="/">
        <input type="submit" value="Back">
      </form><br>
    </body>
  </p>
</html>
)rawliteral";

#endif