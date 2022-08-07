#pragma once

#include <Adafruit_NeoMatrix.h>
#include "ArduinoJson.h"
#include "config.h"

/***
 *       ___     _            
 *      / __|___| |___ _ _ ___
 *     | (__/ _ \ / _ \ '_(_-<
 *      \___\___/_\___/_| /__/
 *                            
 */

#define LED_VERYLOW   32
#define LED_LOW       63
#define LED_MEDIUM   127
#define LED_HIGH     255

#define RGB(r, g, b) (((r & 0xff) << 16) | ((g & 0xff) << 8) | (b &0xff))
#define RGB565(r, g, b) ((((r) / 8) << 11) | (((g) / 4) << 5) | ((b) / 8))
#define RGB_TO_RGB565(rgb) RGB565(((rgb) >> 16) & 0xff, ((rgb) >> 8) & 0xff, (rgb) & 0xff)
#define RGB565_TO_RGB(rgb565) ((((rgb565) << 8) & 0xf80000) + (((rgb565) << 5) & 0xfc00) + (((rgb565) << 3) & 0xf8))

#define LED_WHITE_LOW    RGB565(LED_LOW, LED_LOW, LED_LOW)
#define LED_WHITE_MEDIUM RGB565(LED_MEDIUM, LED_MEDIUM, LED_MEDIUM)

/***
 *      _____                         ___                                    
 *     |_   _|   _ _ __   ___  ___   ( _ )     ___ _ __  _   _ _ __ ___  ___ 
 *       | || | | | '_ \ / _ \/ __|  / _ \/\  / _ \ '_ \| | | | '_ ` _ \/ __|
 *       | || |_| | |_) |  __/\__ \ | (_>  < |  __/ | | | |_| | | | | | \__ \
 *       |_| \__, | .__/ \___||___/  \___/\/  \___|_| |_|\__,_|_| |_| |_|___/
 *           |___/|_|                                                        
 */
typedef struct neoBitmapDef {
    char name[20];
    short width;
    short height;
    const unsigned short *image;
} NeoBitmapDef;

enum AnimateMode {
    None, 
    Text, 
    Bitmap, 
    PseudoLed
};

enum LedMode {
    off, 
    on, 
    blink
};
void JsonMetaOptsLed(JsonObject obj);

enum Alignment {
  left,
  center,
  right,
  scroll,
  bounce
};
void JsonMetaOptsAlignment(JsonObject obj);

/***
 *      _   _            ____  _          _ ____                  _ 
 *     | \ | | ___  ___ |  _ \(_)_  _____| |  _ \ __ _ _ __   ___| |
 *     |  \| |/ _ \/ _ \| |_) | \ \/ / _ \ | |_) / _` | '_ \ / _ \ |
 *     | |\  |  __/ (_) |  __/| |>  <  __/ |  __/ (_| | | | |  __/ |
 *     |_| \_|\___|\___/|_|   |_/_/\_\___|_|_|   \__,_|_| |_|\___|_|
 *                                                                  
 */

typedef struct ledDef {
    LedMode mode;
    int colora;
    int colorb;
} LedDef;

class NeoPixelPanel {
  public:
    int iWidth;
    int iHeight;

    Alignment alignment;
    // Keep coordinates in units of 1/16th pixel. 
    // We will refer to them as points (Pt)
    #define PT_PER_PIXEL 16
    int16_t xPt;
    int16_t yPt;
    // scroll speed - Points per move
    int16_t xPtPerMove;
    int16_t yPtPerMove;
    // Scrolling direction
    int16_t xStepDirection;
    int16_t yStepDirection;

    // Live data
    bool vscroll;
    bool hscroll;
    int totalSteps;
    int currentStep;

    NeoBitmapDef *bmpDef;
    String text;
    int textColorIndex;

    NeoPixelPanel();
    void Reinit(bool anyLedActive);
    void Init(int iWidth, int iHeight, Alignment alignment, int xstep, int ystep, bool anyLedActive);
    bool Update(AnimateMode mode, bool anyLedActive);
};

/***
 *      _   _            ____  _          _ ____  _           _             
 *     | \ | | ___  ___ |  _ \(_)_  _____| |  _ \(_)___ _ __ | | __ _ _   _ 
 *     |  \| |/ _ \/ _ \| |_) | \ \/ / _ \ | | | | / __| '_ \| |/ _` | | | |
 *     | |\  |  __/ (_) |  __/| |>  <  __/ | |_| | \__ \ |_) | | (_| | |_| |
 *     |_| \_|\___|\___/|_|   |_/_/\_\___|_|____/|_|___/ .__/|_|\__,_|\__, |
 *                                                     |_|            |___/ 
 */

class NeopixelDisplay {
    enum AnimateMode mode = AnimateMode::None;
    int startX, endX, config;
    int x;
    int ms;
    int countdown;

    
    String nextText;
    #define LED_PIXELS 2

    LedDef *leds;
    int ledCount;
    NeoPixelPanel panel;

    int ledCountdown;
    int toggleCount;

    Alignment textAlignment;
    Alignment bmpAlignment;
    uint16_t bmpWhich;

    bool dirty;
public:

    NeopixelDisplay();

          
    void SetNextText(String &text);
    String& GetText();
    int ShowText(String text) { return ShowText(text, panel.textColorIndex); }
    int ShowText(String text, int colorIndex) { return ShowText(text, colorIndex, textAlignment); }
    int ShowText(String text, int colorIndex, Alignment alignment);
    int ShowNextText(int colorIndex, Alignment alignment);
    Alignment GetTextAlignment();
    void SetTextAlignment(Alignment alignment);
    void SetTextColor(int value);
    int GetTextColor();

    void LedSetColorA(int index, int color);
    void LedSetColorB(int index, int color);
    int LedGetColorA(int index);
    int LedGetColorB(int index);
    LedDef *GetLeds();
    void SetLed(int index, LedMode mode);

    int GetBitmap() { return bmpWhich; }
    int ShowBitmap(uint16_t which, Alignment alignment);
    int ShowBitmap(uint16_t which) { return ShowBitmap(which, bmpAlignment); }
    Alignment GetBmpAlignment();
    void SetBmpAlignment(Alignment alignment);

    void Invalidate();

    void Tick10ms();
    
private:
    bool AnyLedActive();
    void _UpdateLeds(bool anyLedActive);
    void _DrawLed(int index, int toggle);
};

/***
 *     __   __        _      _    _        
 *     \ \ / /_ _ _ _(_)__ _| |__| |___ ___
 *      \ V / _` | '_| / _` | '_ \ / -_|_-<
 *       \_/\__,_|_| |_\__,_|_.__/_\___/__/
 *                                         
 */

// THE display object
extern NeopixelDisplay display;


/***
 *      ___             _   _             
 *     | __|  _ _ _  __| |_(_)___ _ _  ___
 *     | _| || | ' \/ _|  _| / _ \ ' \(_-<
 *     |_| \_,_|_||_\__|\__|_\___/_||_/__/
 *                                        
 */
void NeoDisplaySetup(int gpio, int matrixW, int matrixH, int panelsW, int panelsH);
void NeoDisplay10msTick();

void JsonMetaOptsColor(JsonObject obj);
void JsonMetaOptsBitmap(JsonObject obj);

/*
void NeoWhiteLevelTest();
void NeoLinePatternTest();
void NeoConcentricBoxes();
void NeoConcentricCircles();
*/
