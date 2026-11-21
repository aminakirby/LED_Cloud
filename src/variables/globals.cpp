#include <globals.h>
#include <functions.h>

CRGB RGBLED[NUM_LEDS];

//Network Parameters
IPAddress gateway(10, 0, 0, 1);
IPAddress subnet(255, 255, 255, 0);
ESP8266WebServer server(80);
WiFiClientSecure client;
WiFiClient espClient;
WiFiServer tcpServer(8080);
PubSubClient mqttClient(espClient);

//MQTT ParametersLED
const char* mqtt_server = "192.168.0.100";
const char* mqtt_username = "mqtt";
const char* mqtt_password = "$p4nkE101";
const char* mqtt_topic_rx_all = "home/condo/clouds";
const char* mqtt_topic_rx_single = "home/condo/" DEVICE_ID;
const char* mqtt_topic_tx = "home/condo/clouds/log";
const int mqtt_port = 1883;

//Mode/sACN/PurpleAir Parameters
uint8_t mode;
uint16_t universe;
uint16_t universeCount = NUM_LEDS / 170 + ((NUM_LEDS % 170) ? 1 : 0);
uint32_t sensor;
ESPAsyncE131 e131(universeCount);

//HTTP Parameters
String purpleAirAPIurl = "https://api.purpleair.com/v1/sensors/";                        //PurpleAir base url.
String sensorNumber = "70169";                                                           //PurpleAir sensor number. (Default: 70169)
String apiKeyQuery = "?api_key=";                                                        //API Key query.
String apiKey;                                                                           //PurpleAir API key. (Default: 3D0F0CEE-32BF-11ED-B5AA-42010A800006)
String host = purpleAirAPIurl + sensorNumber + apiKeyQuery + apiKey;                     //Web Server address to read/write from. "59921" is the Mills College sensor.
String newApiKey;
uint8_t apiKeyLength;
uint16_t pm25;                                                                           

//Color HSV Parameters
uint8_t ledVal[NUM_LEDS], ledOn[NUM_LEDS], ledHue[NUM_LEDS], ledWhite[NUM_LEDS];
uint8_t currentHue;
uint8_t brightnessOn = 1;
uint16_t numFadeLeds = NUM_LEDS/10;                                                      //Maximum number of LEDs on at any given time while in Purple Air mode.
bool rainbowEn;
bool whiteEn;

//Time Parameters
uint32_t timeMilliseconds, ledPastTime, httpPastTime, resetTime;
bool resetEnable;