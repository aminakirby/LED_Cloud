#include <globals.h>
#include <functions.h>

//Webserver Functions
void handleRoot(){                                                                      //Index page.
  server.send(200, "text/html", index_html);
};

void handleGet(){                                                                       //Receives network arguments and writes them to EEPROM.
  String getSSID = server.arg(0);
  String getPassword = server.arg(1);
  uint8_t ssidLength = getSSID.length();
  uint8_t passwordLength = getPassword.length();
  uint8_t ips[16];
  for(uint8_t i = 0; i < 16; i++){ips[i] = server.arg(i+2).toInt();};
  IPAddress ip(ips[0], ips[1], ips[2], ips[3]);
  IPAddress subnet(ips[4], ips[5], ips[6], ips[7]);
  IPAddress gateway(ips[8], ips[9], ips[10], ips[11]);
  IPAddress dns(ips[12], ips[13], ips[14], ips[15]);

  printWiFiInfo(getSSID, getPassword, ip, subnet, gateway);

  EEPROM.write(0, ssidLength);                                     //Writes length of SSID to EEPROM address 0.
  EEPROM.write(ssidLength + 1, passwordLength);                    //Writes length of password to EEPROM address immediately following SSID.
  for(uint8_t i = 0; i < ssidLength; i ++){                        //Writes SSID to EEPROM addresses immediately following address 0.
    EEPROM.write(i + 1, getSSID[i]);
  };                                                              
  for(uint8_t i = 0; i < passwordLength; i ++){                    //Writes password to EEPROM addresses immediately following password length address.
    EEPROM.write(i + 2 + ssidLength, getPassword[i]);
  };
  for(uint8_t i = 0; i < 16; i++){
    EEPROM.write(i + (2 + ssidLength + passwordLength), ips[i]);
  };
  EEPROM.commit();
 
  server.send(200, "text/html", refresh_html);
};

void handleMode(){                                                                       //Receives mode arguments and writes it to EEPROM.
  uint8_t newMode = server.arg(0).toInt();
  EEPROM.write(404, newMode);
  uint32_t modeVal;

  switch(newMode){
    case 0:
      modeVal = server.arg(1).toInt();
      for(uint8_t i = 0; i < 4; i++){EEPROM.write(i + 420, modeVal >> (i * 8));};
      EEPROM.commit();
      Serial.print("Mode set to sACN | Listening on universe ");
      Serial.print(modeVal);
      Serial.println(" | Restart to apply settings.");
      break;
    case 1:
      modeVal = server.arg(2).toInt();
      newApiKey = server.arg(3);
      apiKeyLength = newApiKey.length();
      for(uint8_t i = 0; i < 4; i++){EEPROM.write(i + 410, modeVal >> (i * 8));};
      for(uint8_t i = 0; i < apiKeyLength; i++){EEPROM.write(i + 450, newApiKey[i]);};
      EEPROM.commit();
      Serial.print("Mode set to Purple Air AQI | Monitoring sensor number ");
      Serial.print(modeVal);
      Serial.print(" | Purple Air API Key: ");
      Serial.print(newApiKey);
      Serial.println(" | Restart to apply settings.");
      break;
    case 2:
      modeVal = 0;
      for(uint8_t i = 0; i < 4; i++){EEPROM.write(i + 400, modeVal >> (i * 8));};
      EEPROM.commit();
      Serial.println("Mode set to Color Selector | Restart to apply settings.");
      break;
  }

  server.send(200, "text/html", refresh_html);
};

void handleSensor(){
  server.send(200, "text/html", purpleAir_info);
};

void handleColor(){
  if(mode==2){
    uint32_t modeVal = server.arg(0).toInt();
    for(uint8_t i = 0; i < 4; i++){EEPROM.write(i + 400, modeVal >> (i * 8));};
    EEPROM.commit();
    String color;
    if(modeVal != 8){rainbowEn = false;};
    if(modeVal != 9){whiteEn = false;};
    if(modeVal != 10){brightnessOn = 1;};
    switch(modeVal){
      case 0: color = "Red";    {currentHue = red;};    break;
      case 1: color = "Orange"; {currentHue = orange;}; break;
      case 2: color = "Yellow"; {currentHue = yellow;}; break;
      case 3: color = "Green";  {currentHue = green;};  break;
      case 4: color = "Blue";   {currentHue = blue;};   break;
      case 5: color = "Indigo"; {currentHue = indigo;}; break;
      case 6: color = "Purple"; {currentHue = purple;}; break;
      case 7: color = "Pink";   {currentHue = pink;};   break;
      case 8: color = "Rainbow";{rainbowEn = true;};    break;
      case 9: color = "White";  {whiteEn = true;};      break;
      case 10: color = "Off";   {brightnessOn = 0;};    break;
    };
    Serial.print("Color set to ");
    Serial.print(color);
    Serial.println(".");
  };
  server.send(200, "text/html", refresh_html);
};

void handleClear(){                                                                      //Erases all settings.
  for(uint16_t i = 0; i < 512; i++){                                                     //Erases EEPROM memory.
    EEPROM.write(i, 0);
  };
  EEPROM.commit();
  Serial.println("Network and mode settings cleared.");
  server.send(200, "text/html", refresh_html);
};

void handleRestart(){                                                                    //Restarts ESP8266.
  Serial.println("Restart.");
  resetEnable = true;
  resetTime = millis();
  server.send(200, "text/html", refresh_html);
};

void startAccessPoint(){                                                                 //Enables access point mode if WiFi connection fails.
  WiFi.softAPConfig(gateway, gateway, subnet);
  WiFi.softAP(APSSID, APPSK, 1, 1);
  Serial.println("Access point mode enabled.");
  Serial.print("Access point SSID: ");
  Serial.println(APSSID);
  Serial.print("Access point Password: ");
  Serial.println(APPSK);
};

void startServer(){                                                                      //Starts webserver.
  server.on("/", handleRoot);
  server.on("/get", handleGet);
  server.on("/mode", handleMode);
  server.on("/sensor", handleSensor);
  server.on("/color", handleColor);
  server.on("/clear", handleClear);
  server.on("/restart", handleRestart);
  server.begin();
};
