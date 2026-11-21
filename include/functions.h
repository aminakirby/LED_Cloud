#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void sacnInit();
void e131PullPacket();
uint16_t numLedsOn();
void writeLedsFade();
void writeLedsConst(const uint8_t hue, uint8_t brightness);
uint16_t aqiCalc(float data);
void apiCall();
void purpleAirInit();
void purpleAirRun();
void colorSelectorInit();
void colorSelectorRun();
void printWiFiInfo(String wifiName, String wifiPass, IPAddress ip, IPAddress subnet, IPAddress gateway);
bool startWiFi();
void handleRoot();
void handleGet();
void handleMode();
void handleSensor();
void handleColor();
void handleClear();
void handleRestart();
void tcpListen();
void mqttReconnect();
void mqttCallback(char* topic, byte* payload, uint32_t length);
void startAccessPoint();
void startServer();
void resetCheck();

#endif