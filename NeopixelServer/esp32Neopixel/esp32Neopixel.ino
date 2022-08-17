#include <Adafruit_NeoMatrix.h>

#include "config.h"
#include "neopixelparams.h"

#include "neopixel.h"
#include "neopixelble.h"
#include "neopixeldisplay.h"
#include "neopixelwifi.h"


/***
 *      ____       _               
 *     / ___|  ___| |_ _   _ _ __  
 *     \___ \ / _ \ __| | | | '_ \ 
 *      ___) |  __/ |_| |_| | |_) |
 *     |____/ \___|\__|\__,_| .__/ 
 *                          |_|    
 */
void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  // Serial and hardware init
  Serial.begin(115200);

  // Includes Initializing Preferences
  NeoParamSetup();
  
  // Set up the NeoDisplay EARLY - Others may want to use the display during startup
  NeoDisplaySetup(MATRIX_GPIO);

  NeoBleSetup();
  NeoWifiSetup();

  JsonEncodeNeopixelData();
}

static unsigned long previous10ms = 0; // Last 10ms interval

/***
 *      _                   
 *     | | ___   ___  _ __  
 *     | |/ _ \ / _ \| '_ \ 
 *     | | (_) | (_) | |_) |
 *     |_|\___/ \___/| .__/ 
 *                   |_|    
 */
void loop() {
    unsigned long currentTime = millis();

    NeoBleTick();
    NeoWifiTick();
    NeoParamTick();

    // if it is greater than 10 ms
    if (currentTime - previous10ms > 10) {
      previous10ms = currentTime;
      NeoDisplay10msTick();
    }
}
