#include <globals.h>
#include <functions.h>

//Utility
void resetCheck(){                                                                       //Restarts ESP8266 after 1 second if resetEnable changed to 1.
  if(resetEnable){                                                                      
      uint32_t time = millis();                                                          //Needs 1 second delay to send user back to index page before reset.
      if(time - resetTime > 1000){
        writeLedsConst(0, 0);                                                            //Turns off LEDs before reset.
        ESP.restart();
      };
  };
};