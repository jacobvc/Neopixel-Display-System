#include <Adafruit_NeoMatrix.h>

#include "config.h"
#include "neopixelparams.h"

#include "neopixel.h"
#include "neopixelble.h"
#include "neopixeldisplay.h"
#include "neopixelwifi.h"

uint8_t buttonState;

Sequence sequence;


/***
 *      ____                                         ____  _                 
 *     / ___|  ___  __ _ _   _  ___ _ __   ___ ___  / ___|| |_ ___ _ __  ___ 
 *     \___ \ / _ \/ _` | | | |/ _ \ '_ \ / __/ _ \ \___ \| __/ _ \ '_ \/ __|
 *      ___) |  __/ (_| | |_| |  __/ | | | (_|  __/  ___) | ||  __/ |_) \__ \
 *     |____/ \___|\__, |\__,_|\___|_| |_|\___\___| |____/ \__\___| .__/|___/
 *                    |_|                                         |_|        
 */

int StepBoxes(struct sequenceDef *seq)
{
  display.LedConfig(seq->config - 1, 2 * seq->config, 2 * seq->config + 1);
  display.SetLed(seq->config - 1, LedMode::blink);

  //NeoConcentricBoxes();
  return 0; // ++seq->state >= seq->config;
}
int StepBanner(struct sequenceDef *seq)
{
   return display.ShowNextText(seq->config, Alignment::center);
}
int StepGlyph(struct sequenceDef *seq)
{
//  display_all_bitmaps();
  return  display.ShowBitmap(seq->config, Alignment::center);
  //return true; // ++seq->state >= seq->config;
}
int StepBitmap(struct sequenceDef *seq)
{
  return display.ShowBitmap(seq->config, Alignment::bounce);
  //return true; // ++seq->state >= seq->config;
}


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
  
  // Set up the neo driver EARLY - Others may want to use the display during startup
  NeoDisplaySetup(MATRIX_GPIO, MATRIX_WIDTH, MATRIX_HEIGHT, TILES_WIDE, TILES_HIGH);

  NeoBleSetup();
  NeoWifiSetup();

  JsonEncodeNeopixelData();
}

static unsigned long nextStepTime = 0; // Last second interval
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

    //
    // Process I/O
    //
    if (currentTime > nextStepTime) {
      int delta = sequence.Tick();
      nextStepTime = currentTime + delta;
    }
    // if it is greater than 10 ms
    if (currentTime - previous10ms > 10) {
      previous10ms = currentTime;
      NeoDisplay10msTick();
    }

    //uint8_t btn = digitalRead(BUILTIN_BUTTON);
  /* if (btn != buttonState) {
      SendButtonCharacteristic(!btn);
      Serial.print("BTN ");
      Serial.println(btn);
    }
    buttonState = btn;
  */
}

/***
 *      ____                                         ___                 _ 
 *     / ___|  ___  __ _ _   _  ___ _ __   ___ ___  |_ _|_ __ ___  _ __ | |
 *     \___ \ / _ \/ _` | | | |/ _ \ '_ \ / __/ _ \  | || '_ ` _ \| '_ \| |
 *      ___) |  __/ (_| | |_| |  __/ | | | (_|  __/  | || | | | | | |_) | |
 *     |____/ \___|\__, |\__,_|\___|_| |_|\___\___| |___|_| |_| |_| .__/|_|
 *                    |_|                                         |_|      
 */

Sequence::Sequence() 
    : steps({{ StepBoxes }, { StepBanner }, { StepGlyph }, { StepBitmap }}) {
    stepNumber = 0;
  }
  void Sequence::SetConfig(byte *config) {
    Serial.print("Sequence: ");
    for (int i = 0; i < STEP_COUNT; ++i) {
      steps[i].config = config[i];
      sequenceConfig[i] = config[i];
    }
    for (int i = 0; i < STEP_COUNT; ++i) {
          Serial.print( steps[i].config);
          Serial.print(",");
    }
    Serial.println("");
  }
  byte *Sequence::GetConfig() {
    return sequenceConfig;    
  }    
  
  int Sequence::NextStep()
  {
    int count = 0;

    do {
      int index = stepNumber % STEP_COUNT;
      SequenceDef *seq = &steps[index];
      if (seq->config) {
        int duration = seq->step(seq);
        if (duration >= 0) {
          index = ++stepNumber % STEP_COUNT;
          steps[index].state = 0;
          return duration + 1000;
        }
        else {
          index = ++stepNumber % STEP_COUNT;
          steps[index].state = 0;
        }
      }
      else {
          // Ths non-step is done. Advance and clear state for next
          index = ++stepNumber % STEP_COUNT;
          steps[index].state = 0;
      }
    } while (++count < STEP_COUNT);

    return 0;
  }

  int Sequence::Tick() {
      int delta = NextStep();
      if (delta == 0) {
        if (digitalRead(BUILTIN_LED)) {
          // matrix->fillScreen(LED_WHITE_LOW);
        }
        else {
          //matrix->clear();
        }
        //matrix->show();
      }        
      return delta;
  }
