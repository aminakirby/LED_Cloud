#include <globals.h>
#include <functions.h>

//Purple Air Functions
uint16_t aqiCalc(float data){                                                            //Calculates current PM2.5 based on raw data from Purple Air sensor JSON.
	if(data >= 0.0 && data < 12.0){
    data = ((50.0/12.0) * (data - 0.0)) + 0.0;
    currentHue = green;
  }
  else if(data > 12.1 && data < 35.4){
    data = ((49.0/23.3) * (data - 12.1)) + 51.0;
    currentHue = yellow;
  }
	else if(data > 35.5 && data < 55.4){
    data = ((49.0/19.9) * (data - 35.5)) + 101.0;
    currentHue = orange;
  }
	else if(data > 55.5 && data < 150.4){
    data = ((49.0/94.9) * (data - 55.5)) + 151.0;
    currentHue = red;
  }
	else if(data > 150.5 && data < 250.4){
    data = ((99.0/99.9) * (data - 150.5)) + 201.0;
    currentHue = purple;
  }
	else if(data > 250.5 and data < 350.4){
    data = ((99.0/99.9) * (data - 250.5)) + 301.0;
    currentHue = purple;
  }
	else if(data > 350.5){
    data = ((99.0/149.9) * (data - 350.5)) + 401.0;
    currentHue = purple;
  };
	return (uint16_t)data;
};

void apiCall(){                                                                          //Pulls new sensor data JSON from Purple Air.
  HTTPClient http;
  http.begin(client, host);
  uint32_t httpCode = http.GET();

  if(httpCode == 200){                                                                   //If http payload is received successfully (http code 200).
    String payload = http.getString();                                                   //Transfer the HTTP data into a string.
    const size_t capacity = 4096;                                                        //Allocates memory for the JSON document plus a buffer of 1024.
    DynamicJsonDocument doc(capacity);                                                   //Create JSON document.
    DeserializationError err = deserializeJson(doc, payload);                            //Deserializes and adds http payload to JSON document.
    if (err){                                                                            //Flashes blue 5 times if DeserializationError exists. 
      Serial.print("Deserialization failed: ");
      Serial.println(err.f_str());
      for(uint8_t i=0; i<5; i++){
        writeLedsConst(indigo, 15);
        delay(100);
        writeLedsConst(indigo, 0);
        delay(400);
      };
      http.end();
      return;
    };

    doc.shrinkToFit();                                                                   //Re-allocates excess memory not needed from the JSON document.
    float pmFloat = doc["sensor"]["pm2.5"].as<float>();                                  //Pulls raw "pm2.5" value from PurpleAir JSON.
    pm25 = aqiCalc(pmFloat);                                                             //Calculates "pm2.5" into US EPA format and assigns to pm25 variable.

    Serial.print("Current AQI (PM2.5): ");
    Serial.println(pm25);
  }
  else{                                                                                  //Flashes red 5 times if httpCode != 200.
    Serial.print("HTTP Error: ");
    Serial.println(httpCode);                                     
    for(uint8_t i=0; i<5; i++){
      writeLedsConst(red, 15);
      delay(100);
      writeLedsConst(red, 0);
      delay(400);
    };
  };

  http.end();
};

void purpleAirInit(){                                                                    //Reads sensor number from EEPROM and sets http get URL.
  sensor = EEPROM.read(410) + (EEPROM.read(411) << 8) + (EEPROM.read(412) << 16) + (EEPROM.read(413) << 24);
  for(uint8_t i = 0; i < 36; i ++){apiKey += char(EEPROM.read(i + 450));};
  String newHost = "https://api.purpleair.com/v1/sensors/";
  newHost += sensor;
  newHost += "?api_key=";
  newHost += apiKey;
  host = newHost;

  client.setInsecure();
  client.connect(host, 443);

  Serial.print("Purple Air Sensor: ");
  Serial.println(sensor);
  Serial.print("Pruple Air API Key: ");
  Serial.println(apiKey);
  Serial.print("API Call URL: ");
  Serial.println(host);

  uint32_t currentBrightnessState = EEPROM.read(400) + (EEPROM.read(401) << 8) + (EEPROM.read(402) << 16) + (EEPROM.read(403) << 24);
  if(currentBrightnessState != 10){brightnessOn = 1;}else{brightnessOn = 0;};
};

void purpleAirRun(){                                                                     //Sends http get request every 2 minutes(duration before PurpleAir sensor updates) and fades on LEDs.
  tcpListen();
  timeMilliseconds = millis();
  if(timeMilliseconds - httpPastTime >= 120000){
    httpPastTime = timeMilliseconds;
    apiCall();
  };
  writeLedsFade();                                                                       
};