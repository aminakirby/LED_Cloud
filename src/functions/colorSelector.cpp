#include <globals.h>
#include <functions.h>

//Color Selector Functions
void colorSelectorInit(){
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(mqttCallback);

  uint32_t hue = EEPROM.read(400) + (EEPROM.read(401) << 8) + (EEPROM.read(402) << 16) + (EEPROM.read(403) << 24);
  Serial.print("Color Selector Color: ");
  switch(hue){
    case 0:  currentHue = red;    Serial.println("Red");     break;
    case 1:  currentHue = orange; Serial.println("Orange");  break;
    case 2:  currentHue = yellow; Serial.println("Yellow");  break;
    case 3:  currentHue = green;  Serial.println("Green");   break;
    case 4:  currentHue = blue;   Serial.println("Blue");    break;
    case 5:  currentHue = indigo; Serial.println("Indigo");  break;
    case 6:  currentHue = purple; Serial.println("Purple");  break;
    case 7:  currentHue = pink;   Serial.println("Pink");    break;
    case 8:  rainbowEn = true;    Serial.println("Rainbow"); break;
    case 9:  whiteEn = true;      Serial.println("White");   break;
    case 10: brightnessOn = 0;    Serial.println("Off");     break;
  };
};

void colorSelectorRun(){
  if(!mqttClient.connected()){mqttReconnect();};
  mqttClient.loop();
  tcpListen();
  timeMilliseconds = millis();
  writeLedsFade();
};