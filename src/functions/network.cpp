#include <globals.h>
#include <functions.h>

//Network Functions
void printWiFiInfo(String wifiName, String wifiPass, IPAddress ip, IPAddress subnet, IPAddress gateway){
  Serial.print("SSID: ");
  Serial.println(wifiName);
  Serial.print("Password: ");
  Serial.println(wifiPass);
  if(ip){
    Serial.print("IP Address: ");
    Serial.println(ip);
    Serial.print("Subnet: ");
    Serial.println(subnet);
    Serial.print("Gateway: ");
    Serial.println(gateway);
  };
};

bool startWiFi(){                                                                                                //Reads saved mode, SSID/password and static IP info from EEPROM and connects WiFi.
  uint8_t ssidLength = EEPROM.read(0);
  uint8_t passwordLength = EEPROM.read(ssidLength + 1);
  String wifiName;
  String wifiPass;

  if(ssidLength != 0 && passwordLength != 0){
    for(uint8_t i = 0; i < ssidLength; i++){wifiName += char(EEPROM.read(i+1));};
    for(uint8_t i = 0; i < passwordLength; i++){wifiPass += char(EEPROM.read(i+2+ssidLength));};
  };

  uint8_t ips[16];
  for(uint8_t i = 0; i < 16; i++){ips[i] = EEPROM.read(i + (2 + ssidLength + passwordLength));};
  IPAddress ip(ips[0], ips[1], ips[2], ips[3]);
  IPAddress subnet(ips[4], ips[5], ips[6], ips[7]);
  IPAddress gateway(ips[8], ips[9], ips[10], ips[11]);
  IPAddress dns(ips[12], ips[13], ips[14], ips[15]);

  printWiFiInfo(wifiName, wifiPass, ip, subnet, gateway);

  mode = EEPROM.read(404);
  switch(mode){
    case 0: sacnInit(); break;
    case 1: purpleAirInit(); break;
    case 2: colorSelectorInit(); break;
  };

  if(ip){WiFi.config(ip, dns, gateway, subnet);};                                                                //If static IP info exists, configures WiFi with it.
  WiFi.begin(wifiName, wifiPass);
  Serial.print("Connecting to WiFi...");

  uint8_t attempt = 0;
  while (WiFi.status() != WL_CONNECTED && attempt < 20){                                                         //Checks WiFi.status() 20 times before returning that WiFi failed.
    writeLedsConst(yellow, 15);
    delay(100);
    writeLedsConst(yellow, 0);
    delay(400);
    Serial.print(".");
    attempt++;
  };

  if(WiFi.status() != WL_CONNECTED){
    writeLedsConst(red, 15);
    delay(500);
    writeLedsConst(red, 0);
    delay(2000);
    Serial.println("");
    Serial.println("WiFi connection failed. Disconnecting.");
    WiFi.disconnect(false);
    return false;
  }
  else{
    writeLedsConst(green, 15);
    delay(500);
    writeLedsConst(green, 0);
    delay(2000);
    Serial.println("");
    Serial.println("WiFi connection successful.");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    return true;
  };
};

void mqttReconnect(){
  while (!mqttClient.connected()){
    if(mqttClient.connect("sensor." DEVICE_ID, mqtt_username, mqtt_password)){
      Serial.println("Connected to MQTT broker");
      mqttClient.subscribe(mqtt_topic_rx_single);
      mqttClient.subscribe(mqtt_topic_rx_all);
      mqttClient.publish(mqtt_topic_tx, DEVICE_ID " connected!");
    }
    else{
      Serial.println("Attempt to connect to MQTT server failed. Retrying...");
      delay(5000);
    }
  }
}

void mqttCallback(char* topic, byte* payload, uint32_t length) {
  String message = "";

  for (uint32_t i = 0; i < length; i++) {message += (char)payload[i];}
  Serial.print("MQTT Message received: ");
  Serial.println(message);

  if (message.length() > 0) {
    uint32_t messageInt = message.toInt();

    if (messageInt >= 0 && messageInt <= 10) {
      uint32_t modeVal = messageInt;
      for (uint8_t i = 0; i < 4; i++) { EEPROM.write(i + 400, modeVal >> (i * 8)); }
      EEPROM.commit();
      if (modeVal != 8) { rainbowEn = false; }
      if (modeVal != 9) { whiteEn = false; }
      if (modeVal != 10) { brightnessOn = 1; }
      if (mode == 2 || (mode == 1 && modeVal == 10)) {
        String color = "";
        switch (modeVal) {
          case 0: color = "Red";     currentHue = red;    break;
          case 1: color = "Orange";  currentHue = orange; break;
          case 2: color = "Yellow";  currentHue = yellow; break;
          case 3: color = "Green";   currentHue = green;  break;
          case 4: color = "Blue";    currentHue = blue;   break;
          case 5: color = "Indigo";  currentHue = indigo; break;
          case 6: color = "Purple";  currentHue = purple; break;
          case 7: color = "Pink";    currentHue = pink;   break;
          case 8: color = "Rainbow"; rainbowEn = true;    break;
          case 9: color = "White";   whiteEn = true;      break;
          case 10: color = "Off";    brightnessOn = 0;    break;
        }
        Serial.print("Color set to ");
        Serial.print(color);
        Serial.println(".");
      }
    } else {
      mqttClient.publish(mqtt_topic_tx, DEVICE_ID "MQTT Packet Error: Invalid data format.");
      Serial.println("MQTT Packet Error: Invalid data format.");
    }
  } else {
    mqttClient.publish(mqtt_topic_tx, DEVICE_ID "MQTT Packet Error: No data or data is unreadable.");
    Serial.println("MQTT Packet Error: No data or data is unreadable.");
  }
}

void tcpListen(){
  WiFiClient client = tcpServer.accept();

  if(client){
    Serial.println("TCP client connected.");
    String message, color = "";
    uint32_t messageInt;

    if(client.available()){
      message = client.read();
    }
    else{
      Serial.println("TCP Packet Error: No data.");
      Serial.println("Disconnecting client...");
      client.stop();
      return;
    }

    if(message.length() > 0){
      messageInt = message.toInt() - 48;                                                        //Minus 48 to convert ASCII to integer.
    }
    else{
      Serial.println("TCP Packet Error: No data or data is unreadable.");
      Serial.println("Disconnecting client...");
      client.stop();
      return;
    }

    if(messageInt >= 0 && messageInt <= 10){
      uint32_t modeVal = messageInt;
      for(uint8_t i = 0; i < 4; i++){EEPROM.write(i + 400, modeVal >> (i * 8));};
      EEPROM.commit();
      if(modeVal != 8){rainbowEn = false;};
      if(modeVal != 9){whiteEn = false;};
      if(modeVal != 10){brightnessOn = 1;};
      if(mode == 2 || (mode == 1 && modeVal == 10)){
        switch(modeVal){
          case 0: color = "Red";     currentHue = red;    break;
          case 1: color = "Orange";  currentHue = orange; break;
          case 2: color = "Yellow";  currentHue = yellow; break;
          case 3: color = "Green";   currentHue = green;  break;
          case 4: color = "Blue";    currentHue = blue;   break;
          case 5: color = "Indigo";  currentHue = indigo; break;
          case 6: color = "Purple";  currentHue = purple; break;
          case 7: color = "Pink";    currentHue = pink;   break;
          case 8: color = "Rainbow"; rainbowEn = true;    break;
          case 9: color = "White";   whiteEn = true;      break;
          case 10: color = "Off";    brightnessOn = 0;    break;
        };
        Serial.print("Color set to ");
        Serial.print(color);
        Serial.println(".");
      };

      Serial.println("Disconnecting client...");
      client.stop();
      return;
    }
    else{
      Serial.println("TCP Packet Error: Invalid data format.");
      Serial.println("Disconnecting client...");
      client.stop();
      return;
    };
  };
};