#include <globals.h>
#include <functions.h>

//LED functions.
uint16_t numLedsOn(){                                                                     //Returns number of LEDs currently turned on.
  uint16_t onTotal = 0;
  for(uint16_t i=0; i<NUM_LEDS; i++){
    onTotal += ledOn[i];
  };
  return onTotal;
}

void writeLedsFade(){                                                                    //Randomly selects LEDs to fade in/out with color of AQI sensor.
  if(timeMilliseconds - ledPastTime >= 10){
    ledPastTime = timeMilliseconds;

    if(numLedsOn() <= numFadeLeds){                                                      //If number of LEDs on is less than maximum, randomly selects an off LED to turn on.
      uint16_t selectedLed = random(NUM_LEDS);
      while(ledOn[selectedLed] || ledVal[selectedLed] > 0){selectedLed = random(NUM_LEDS);};
      ledOn[selectedLed] = 1;
      if(rainbowEn){currentHue += 1;};
      if(whiteEn){ledWhite[selectedLed] = 0;}else{ledWhite[selectedLed] = 255;};
      ledHue[selectedLed] = currentHue;
    };

    for(uint16_t i=0; i<NUM_LEDS; i++){                                                   //Fades up turned-on LEDs to maximum brightness or fades down if LEDs are already at maximum brightness.
      if(ledOn[i]){
        if(ledVal[i]<255){
          ledVal[i] += 1;
        }
        else{ledOn[i] = 0;};
        if(ledVal[i]>255){
          ledVal[i] = 255;
        };
      }
      else{
        if(ledVal[i] > 0){
          ledVal[i] -= 1;
        };
      };
      if(ledVal[i] < 0){ledVal[i] = 0;};
    };
  };

  for(uint16_t i=0; i<NUM_LEDS; i++){
    RGBLED[i] = CHSV(ledHue[i], ledWhite[i], (ledVal[i] * brightnessOn));
  };

  FastLED.show();
}

void writeLedsConst(const uint8_t hue, uint8_t brightness){                              //Sets all LEDs to specified hue and brightness.
  for(uint16_t i=0; i<NUM_LEDS; i++){
    RGBLED[i] = CHSV(hue, 255, brightness);
    ledVal[i] = 0;
    ledOn[i] = 0;
  };
  FastLED.show();
};