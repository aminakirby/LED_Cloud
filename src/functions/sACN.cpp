#include <globals.h>
#include <functions.h>

//sACN Functions
void sacnInit(){
  universe = EEPROM.read(420) + (EEPROM.read(421) << 8);                                 //Reads universe number saved in EEPROM.

  WiFi.mode(WIFI_STA);                                                                   //Sets WiFi to station mode.

  Serial.print("sACN Universe: ");
  Serial.println(universe);
};

void e131PullPacket(){
  if (!e131.isEmpty()){
    e131_packet_t packet;
    e131.pull(&packet);                                                                  //Pull packet from ring buffer

    for(int i = 0; i < universeCount; i ++){
      if(htons(packet.universe) == universe+i){
          for(int channel = ((NUM_LEDS/universeCount)*i); channel < ((NUM_LEDS/universeCount)*(i+1)); channel ++){
              RGBLED[channel] = CRGB(
                  packet.property_values[(channel * 3) + 1], 
                  packet.property_values[(channel * 3) + 2], 
                  packet.property_values[(channel * 3) + 3]
              );
          };
      };
    }

    FastLED.show();
  }
};