#include <globals.h>
#include <functions.h>

void setup() {
  //FastLED.addLeds<LED_TYPE, DATA_PIN, CLOCK_PIN, COLOR_ORDER>(RGBLED, NUM_LEDS);                 //For LEDs that need clock pin.
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(RGBLED, NUM_LEDS);                              //For LEDs that don't need clock pin.
  Serial.begin(115200);
  EEPROM.begin(512);
  
  WiFi.setAutoConnect(false);                                                                      //Disables WiFi auto-connect to last connected network.
  if(!startWiFi()){startAccessPoint();}                                                            //If WiFi doesn't connect, then enables Access Point mode.
  else{
    switch(mode){
      case 0: e131.begin(E131_MULTICAST, universe, universeCount); break;
      case 1: apiCall(); break;
      case 2: break;
    };                                                                                             //If Purple Air mode, makes first API call.
  };

  if(mode != 0){tcpServer.begin();};
  startServer();                                                                                   //Starts webserver.

  ArduinoOTA.setPort(OTA_PORT);
  ArduinoOTA.begin();
}

void loop() {
  server.handleClient();
  switch (mode){
    case 0: e131PullPacket(); break;
    case 1: purpleAirRun(); break;
    case 2: colorSelectorRun(); break;
  };
  resetCheck();
  ArduinoOTA.handle();
}