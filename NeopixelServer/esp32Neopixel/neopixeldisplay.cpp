#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>

#include "config.h"
#include "neopixelparams.h"
#include "neopixeldisplay.h"

// Choose your prefered pixmap
#include "assets/heart24.h"
#include "assets/yellowsmiley24.h"
#include "assets/bluesmiley24.h"
#include "assets/smileytongue24.h"

#include "assets/RgbGlyphs.h"

// Define this to use the "included" custom font (else use builtin font)
#define CUSTOM_FONT

#ifdef CUSTOM_FONT
// Created by https://gfx.stfw.org/
// OR got from https://github.com/robjen/GFX_fonts
#include "assets/Font5x7Fixed.h"
#define TEXTY 7
#else
#define TEXTY 0
#endif

#define INDIGO 0x5003
#define VIOLET 0x8015
#define ORANGE 0xfd00

Adafruit_NeoMatrix *matrix;
NeopixelDisplay display;

int width;
int height;

int rainbow[] = {
  RGB565(255, 0, 0), // red
  ORANGE, // orange
  RGB565(255, 255, 0), // yellow
  RGB565(0, 255, 0), // green
  RGB565(0, 0, 255), // blue
  INDIGO, // indigo
  VIOLET, // violet
};

#define RAINBOW 1
#define FIRST_INDEXED_COLOR 2
#define COLOR_COUNT  (sizeof(rainbow) / sizeof(rainbow[0]))

void JsonMetaOptsColor(JsonObject obj)
{
  obj["name"] = "clr-opts";
  obj["type"] = "opts";
  JsonArray arr = obj.createNestedArray("values");
  
  arr[0]="off";
  arr[1]="Rainbow";
  arr[2]="Red";
  arr[3]="Orange";
  arr[4]="Yellow";
  arr[5]="Green";
  arr[6]="Blue";
  arr[7]="Indigo";
  arr[8]="Violet";
}

int16_t GetIndexedColor(int index)
{
  if (index < FIRST_INDEXED_COLOR || index >= COLOR_COUNT + FIRST_INDEXED_COLOR) {
    return LED_WHITE_MEDIUM;
  }
  else {
    return rainbow[index - FIRST_INDEXED_COLOR];
  }
}

NeoBitmapDef loadedBitmaps[] = {
  { "Heart", 24, 24, heart24 },
  { "Yello Smiley", 24, 24, yellowSmiley24 },
  { "Blue Smiley", 24, 24, blueSmiley24 },
  { "Smiley Tongue", 24, 24, smileyTongue24 },
  { "Small Smiley", 8, 8, rgb8x8Glyphs[10] },
};

void JsonMetaOptsBitmap(JsonObject obj)
{
  obj["name"] = "bmp-opts";
  obj["type"] = "opts";
  JsonArray arr = obj.createNestedArray("values");

  arr[0]="off";
  for (int i = 0; i < sizeof(loadedBitmaps) / sizeof(loadedBitmaps[0]); ++i) {
   arr[i + 1]= loadedBitmaps[i].name;
  }
}

NeoBitmapDef *GetBitmapByIndex(int index)
{
  if (index >= 1 && index <=  sizeof(loadedBitmaps) / sizeof(loadedBitmaps[0])) {
    return &loadedBitmaps[index - 1];
  }
  return NULL;
}

void JsonMetaOptsAlignment(JsonObject obj)
{
  obj["name"] = "align-opts";
  obj["type"] = "opts";
  JsonArray arr = obj.createNestedArray("values");

  arr[0]="left";
  arr[1]="center";
  arr[2]="right";
  arr[3]="scroll";
  arr[4]="bounce";
}

void JsonMetaOptsLed(JsonObject obj)
{
  obj["name"] = "led-opts";
  obj["type"] = "opts";
  JsonArray arr = obj.createNestedArray("values");

  arr[0]="off";
  arr[1]="on";
  arr[2]="blink";
}

/***
 *               _               -
 *      ___  ___| |_ _   _ _ __  
 *     / __|/ _ \ __| | | | '_ \ 
 *     \__ \  __/ |_| |_| | |_) |
 *     |___/\___|\__|\__,_| .__/ 
 *                        |_|    
 */
void NeoDisplaySetup(int gpio, int matrixW, int matrixH, int panelsW, int panelsH)
{
    width = matrixW * panelsW;
    height = matrixH * panelsH;

  matrix = new Adafruit_NeoMatrix(matrixW, matrixH, panelsW, panelsH, gpio,
    NEO_MATRIX_TOP       + NEO_MATRIX_LEFT 
    + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG
    + NEO_TILE_TOP       + NEO_TILE_LEFT
    + NEO_TILE_ROWS      + NEO_TILE_PROGRESSIVE,
    NEO_GRB              + NEO_KHZ800 );

  matrix->begin();
  #ifdef CUSTOM_FONT
    matrix->setFont(&Font5x7Fixed);
  #endif
    matrix->setTextWrap(false);
    matrix->setBrightness(BRIGHTNESS);
    // Test full bright of all LEDs. If brightness is too high
    // for your current limit (i.e. USB), decrease it.
    matrix->fillScreen(LED_WHITE_LOW);
    matrix->show();
    delay(3000);
    matrix->clear();
}

/***
 *      _____ _      _    
 *     |_   _(_) ___| | __
 *       | | | |/ __| |/ /
 *       | | | | (__|   < 
 *       |_| |_|\___|_|\_\
 *                        
 */
void NeoDisplay10msTick()
{
  display.Tick10ms();
}

/***
 *      _  _         ___ _         _ ___               _   ___            _ 
 *     | \| |___ ___| _ (_)_ _____| | _ \__ _ _ _  ___| | |_ _|_ __  _ __| |
 *     | .` / -_) _ \  _/ \ \ / -_) |  _/ _` | ' \/ -_) |  | || '  \| '_ \ |
 *     |_|\_\___\___/_| |_/_\_\___|_|_| \__,_|_||_\___|_| |___|_|_|_| .__/_|
 *                                                                  |_|     
 */

NeoPixelPanel::NeoPixelPanel()
{
  vscroll = true;
  hscroll = true;
  textColorIndex = 0;
  alignment = Alignment::center;
}

void NeoPixelPanel::Reinit(bool anyLedActive) 
{
    // Calculate xPt assuming panel starts at x = 0
    if (iHeight < height) {
      yPt = 0;
      vscroll = false;
    }
    else {
      yPt = max(0, (height - iHeight) / 2) * PT_PER_PIXEL;
      vscroll = true;
    }
    hscroll = (iWidth < width) ? false : true;

    switch(alignment) {
      case Alignment::center:
        xPt = (iWidth < width) ? ((width - iWidth) / 2) * PT_PER_PIXEL : 4 * PT_PER_PIXEL;
        break;
      case Alignment::left:
        xPt = (iWidth < width) ? 0 : 4 * PT_PER_PIXEL;
        break;
      case Alignment::right:
        xPt = (iWidth < width) ? ((width - iWidth - (anyLedActive ? 2 : 0)) * PT_PER_PIXEL)
           : 2 * PT_PER_PIXEL;
        break;
      case Alignment::scroll:
        xPt = (width - 8) * PT_PER_PIXEL;
        hscroll = true;
        break;
      case Alignment::bounce:
        xPt = (iWidth < width) ? ((width - iWidth) / 2) * PT_PER_PIXEL : 4 * PT_PER_PIXEL;
        hscroll = true;
        vscroll = true;
        break;
    }
   
    currentStep = 0;
}

void NeoPixelPanel::Init(int iWidth, int iHeight, Alignment alignment, int xstep, int ystep, bool anyLedActive) 
{
    this->iWidth = iWidth;
    this->iHeight = iHeight;
    this->alignment = alignment;
 
 // scroll speed in 1/16th
    xPtPerMove = xstep;
    yPtPerMove = ystep;
    // scroll down and right by moving upper left corner off screen 
    // more up and left (which means negative numbers)
    xStepDirection = -1;
    yStepDirection = -1;
   
    Reinit(anyLedActive);
  }

bool NeoPixelPanel::Update(AnimateMode mode, bool anyLedActive)
{
  //if (currentStep < totalSteps) {
      bool updDir = false;
      int space = 0;
    
      matrix->clear();
      if (mode == AnimateMode::Bitmap) {
        if (bmpDef == NULL) {
            return false;
        }
        else {
            matrix->drawRGBBitmap(xPt / PT_PER_PIXEL + (anyLedActive ? 2 : 0), 
              yPt / PT_PER_PIXEL, bmpDef->image, iWidth, iHeight);
            space = 2;
        }
      }
      else if (mode == AnimateMode::Text) {
        if (textColorIndex == 0) {
            return false;
        }
        matrix->setCursor(xPt / PT_PER_PIXEL + (anyLedActive ? 2 : 0),
          (yPt / PT_PER_PIXEL) + TEXTY);
        if (textColorIndex == RAINBOW) {
          for (uint8_t index = 0; index < text.length(); ++index) {
            matrix->setTextColor(GetIndexedColor((index % COLOR_COUNT) + FIRST_INDEXED_COLOR));
            matrix->print(text[index]);
          }
        }
        else {
          matrix->setTextColor(GetIndexedColor(textColorIndex));
          matrix->print(text);
        }
        space = 2;
      }
      else {
        return false;
      }
       
      // Only pan if the display size is smaller than the pixmap
      // but not if the difference is too small or it'll look bad.
      if (alignment == Alignment::scroll) {
        // Scrolling
          xPt += xPtPerMove * xStepDirection;
          if (xPt <= - (iWidth - 3) * PT_PER_PIXEL) { xPt = (width - 8) * PT_PER_PIXEL; };
      }
      else if (iWidth - width > space && hscroll) {
          // content wider than display
          xPt += xPtPerMove * xStepDirection;
          if (xPt >= 32) { xStepDirection = -1; updDir = true ; };
          // we don't go negative past right corner, go back positive
          if (xPt <= ((width - iWidth - 1 - (anyLedActive ? 2 : 0)) * PT_PER_PIXEL))
                            { xStepDirection = 1;  updDir = true ; };
      }
      else if (width > iWidth && hscroll) {
          // Content narrower than display
          xPt += xPtPerMove * xStepDirection;
          // Reverse when edge leaves panel
          // Deal with bouncing off the 'walls'
          if (xPt >= (width - iWidth - (anyLedActive ? 2 : 0)) * PT_PER_PIXEL) 
                        { xStepDirection = -1; updDir = true ; };
          if (xPt <= 0) { xStepDirection =  1; updDir = true ; };
      }
      if (iHeight - height > space && vscroll) {
          // content taller than display
          yPt += yPtPerMove * yStepDirection;
          // we shouldn't display past left corner, reverse direction.
          if (yPt >= 0) { yStepDirection = -1; updDir = true ; };
          if (yPt <= ((height - iHeight) * PT_PER_PIXEL)) 
                            { yStepDirection = 1;  updDir = true ; };
      }
      if (height > iHeight && vscroll) {
          // Content shorter than display
          yPt += yPtPerMove * yStepDirection;
          if (yPt >= (height - iHeight) * PT_PER_PIXEL)
                        { yStepDirection = -1; updDir = true ; };
          if (yPt <= 0) { yStepDirection =  1; updDir = true ; };
      }
      
      if (updDir && mode == AnimateMode::Bitmap) {
          // Make the bitmap more interesting (but not text)
          // Add -1, 0 or 1 but constrain result
          xPtPerMove = constrain(xPtPerMove + random(-1, 2), 3, 16);
          yPtPerMove = constrain(yPtPerMove + random(-1, 2), 3, 16);
      }
      currentStep++;
      return true;
    //}
    //else {
    //  if (mode == AnimateMode::Text) {
    //    text = "";
    //  }
    //  return false;
    //}
}


/***
 *      _  _              _         _ ___  _         _             ___            _ 
 *     | \| |___ ___ _ __(_)_ _____| |   \(_)____ __| |__ _ _  _  |_ _|_ __  _ __| |
 *     | .` / -_) _ \ '_ \ \ \ / -_) | |) | (_-< '_ \ / _` | || |  | || '  \| '_ \ |
 *     |_|\_\___\___/ .__/_/_\_\___|_|___/|_/__/ .__/_\__,_|\_, | |___|_|_|_| .__/_|
 *                  |_|                        |_|          |__/            |_|     
 */

NeopixelDisplay::NeopixelDisplay()
{
  dirty = false;
  countdown = ledCountdown = toggleCount = 0;
  ms = 10;
  x = -1000;

  bmpAlignment = Alignment::bounce;
  ledCount = LedsCount();
  leds = new LedDef[ledCount];
  memset(leds, 0, ledCount);
  for (int i = 0; i < ledCount; ++i){
    leds[i].mode = LedMode::off;
  }
  leds[0].colora = RGB565(0, 255, 0); 
  leds[0].colorb = RGB565(0, 255, 0); 
  leds[1].colora = RGB565(255, 0, 0); 
  leds[1].colorb = RGB565(255, 0, 0); 
  leds[2].colora = RGB565(0, 255, 0); 
  leds[2].colorb = RGB565(0, 0, 255); 
  leds[3].colora = RGB565(0, 255, 0); 
  leds[3].colorb = RGB565(255, 0, 0); 
}

/***
 *      _____        _   
 *     |_   _|____ _| |_ 
 *       | |/ -_) \ /  _|
 *       |_|\___/_\_\\__|
 *                       
 */

int NeopixelDisplay::ShowText(String text, int colorIndex, Alignment alignment) {
    panel.text = text;
    textAlignment = alignment;
    panel.textColorIndex = colorIndex;
    matrix->clear();

    if ((text == "" || colorIndex == 0) && mode == AnimateMode::Text) {
      // Showing text and changing to OFF
      mode = AnimateMode::None;
      Invalidate();
      return 0;
    }
    else if (colorIndex != 0) {
      // Turn off bitmap
      bmpWhich = 0;
      // Display text
      matrix->setTextWrap(false);  // we don't wrap text so it scrolls nicely
      matrix->setTextSize(1);
      matrix->setRotation(0);

      int16_t x1, y1;
      uint16_t w, h;
      // Get bounds assuming text may start at third pixel
      matrix->getTextBounds(text, 2, 0, &x1, &y1, &w, &h);
      panel.Init(w, h, alignment, 2, 1, AnyLedActive());

      panel.totalSteps = 300;
      mode = AnimateMode::Text;

      return panel.totalSteps * ms;
    }
    return 0;
}

String& NeopixelDisplay::GetText()
{
   return panel.text; 
   }  

void NeopixelDisplay::SetNextText(String &text)
{
   nextText = text; 
}  
int NeopixelDisplay::ShowNextText(int colorIndex, Alignment alignment) 
{
   return ShowText(nextText, colorIndex, alignment); 
}
Alignment NeopixelDisplay::GetTextAlignment()
{
    return textAlignment;
}
void NeopixelDisplay::SetTextAlignment(Alignment alignment)
{
    textAlignment = alignment;
    if (mode == AnimateMode::Text) {
      ShowText(panel.text, panel.textColorIndex, alignment);
    }
}
void NeopixelDisplay::SetTextColor(int value)
{
    panel.textColorIndex =  value;
    if (mode != AnimateMode::Text && value != 0) {
      ShowText(panel.text, value, textAlignment);
    }
}
int NeopixelDisplay::GetTextColor()
{
  return panel.textColorIndex;
}

/***
 *      ___ _ _                   
 *     | _ |_) |_ _ __  __ _ _ __ 
 *     | _ \ |  _| '  \/ _` | '_ \
 *     |___/_|\__|_|_|_\__,_| .__/
 *                          |_|   
 */

// Scroll within big bitmap so that all if it becomes visible or bounce a small one.
// If the bitmap is bigger in one dimension and smaller in the other one, it will
// be both panned and bounced in the appropriate dimensions.
int NeopixelDisplay::ShowBitmap(uint16_t which, Alignment alignment) {
    bmpAlignment = alignment;
    bmpWhich = which;
    
    panel.bmpDef = GetBitmapByIndex(which);
    if (panel.bmpDef == NULL) {
      return 0;
    }
    // keep integer math, deal with values 16 times too big
    // start by showing upper left of big bitmap or centering if the display is big
    panel.Init(panel.bmpDef->width, panel.bmpDef->height, alignment, 4, 3, AnyLedActive());

    panel.totalSteps = 300;
    mode = AnimateMode::Bitmap;
    // Turn off text color
    panel.textColorIndex = 0;
    return panel.totalSteps * ms;
}
Alignment NeopixelDisplay::GetBmpAlignment()
{
    return bmpAlignment;
}
void NeopixelDisplay::SetBmpAlignment(Alignment alignment)
{
    bmpAlignment = alignment;
    if (mode == AnimateMode::Bitmap) {
      ShowBitmap(bmpWhich, alignment);
    }
}

/***
 *      _           _    
 *     | |   ___ __| |___
 *     | |__/ -_) _` (_-<
 *     |____\___\__,_/__/
 *                       
 */

LedDef *NeopixelDisplay::GetLeds()
{
  return leds;
}
void NeopixelDisplay::SetLed(int index, LedMode mode)
{
  bool anyActive = AnyLedActive();
  leds[index].mode = mode;
  if (mode == LedMode::off) {
    _DrawLed(index, -1);
  }
  if (anyActive != AnyLedActive()) {
    panel.Reinit(!anyActive);
  }
  //  Invalidate();
}
void NeopixelDisplay::LedSetColorA(int index, int color)
{  
      struct ledDef *aled = &leds[index];
      aled->colora = RGB_TO_RGB565(color);
}
void NeopixelDisplay::LedSetColorB(int index, int color)
{
      struct ledDef *aled = &leds[index];
      aled->colorb = RGB_TO_RGB565(color);
}

int NeopixelDisplay::LedGetColorA(int index)
{
    return RGB565_TO_RGB(leds[index].colora);
}
int NeopixelDisplay::LedGetColorB(int index)
{
    return RGB565_TO_RGB(leds[index].colorb);
}

void NeopixelDisplay::_UpdateLeds(bool anyLedActive)
{
    for (int le = 0; le < ledCount; ++le) {
      struct ledDef *aled = &leds[le];
      if (aled->mode == LedMode::blink)  {
        if (toggleCount & 1) {
          _DrawLed(le, -1);
        }
        else {
          _DrawLed(le, toggleCount & 2);
        }
      }
      else if (aled->mode == LedMode::on) {
        _DrawLed(le, toggleCount & 2);
      }
      else if (anyLedActive) {
          // Update OFF when any LEDs are active
          _DrawLed(le, -1);
      }
    }      
    Invalidate();
}

bool NeopixelDisplay::AnyLedActive() {
    for (int le = 0; le < ledCount; ++le) {
      if (leds[le].mode != LedMode::off)  {
        return true;        
      }        
    }   
return false;       
}
void NeopixelDisplay::_DrawLed(int index, int toggle) {
    int y = index * LED_PIXELS;
    int x = 0;
    for (int i = 0; i < LED_PIXELS; ++i) {
      for (int j = 0; j < LED_PIXELS; ++j) {
        if (toggle >= 0) {
          if ((i + j + toggle / 2) & 1) {
            matrix->drawPixel(j + x, i + y, leds[index].colora);
          }
          else {
          // Alternate blink
            matrix->drawPixel(j + x, i + y, leds[index].colorb);
          }
        }
        else {
          matrix->drawPixel(j + x, i + y, 0);
        }
      }
    }
}

/***
 *      ___                         _   
 *     / __|_  _ _ __ _ __  ___ _ _| |_ 
 *     \__ \ || | '_ \ '_ \/ _ \ '_|  _|
 *     |___/\_,_| .__/ .__/\___/_|  \__|
 *              |_|  |_|                
 */

void NeopixelDisplay::Invalidate()
{
  dirty = true;
}

void NeopixelDisplay::Tick10ms()
{
  countdown -= 10;
  ledCountdown -= 10;
  
  bool anyLedActive = false;
  if (countdown <= 0 || ledCountdown <= 0) {
    anyLedActive = AnyLedActive();
  }    
  if (countdown <= 0) {
    if (!panel.Update(mode, anyLedActive)) {
      matrix->clear();
      mode = AnimateMode::None;
    }
    _UpdateLeds(anyLedActive);
    Invalidate();

    countdown = ms;
  }
  if (ledCountdown <= 0) {
    ++toggleCount;
    _UpdateLeds(anyLedActive);
    ledCountdown = 500;
  }
  if (dirty) {
    dirty = false;
    matrix->show();
  }
}

#if 0

/***
 *      ____                         ____  _       
 *     / ___|  ___ _ __ __ _ _ __   | __ )(_)_ __  
 *     \___ \ / __| '__/ _` | '_ \  |  _ \| | '_ \ 
 *      ___) | (__| | | (_| | |_) | | |_) | | | | |
 *     |____/ \___|_|  \__,_| .__/  |____/|_|_| |_|
 *                          |_|                    
 */

/***
 *      ____  _           _               _____                     
 *     |  _ \(_)___ _ __ | | __ _ _   _  |  ___|   _ _ __   ___ ___ 
 *     | | | | / __| '_ \| |/ _` | | | | | |_ | | | | '_ \ / __/ __|
 *     | |_| | \__ \ |_) | | (_| | |_| | |  _|| |_| | | | | (__\__ \
 *     |____/|_|___/ .__/|_|\__,_|\__, | |_|   \__,_|_| |_|\___|___/
 *                 |_|            |___/                             
 */
// Fill the screen with multiple levels of white to gauge the quality
void NeoWhiteLevelTest() {
    matrix->clear();
    matrix->fillRect(0,0, width,height, LED_WHITE_HIGH);
    matrix->drawRect(1,1, width-2,height-2, LED_WHITE_MEDIUM);
    matrix->drawRect(2,2, width-4,height-4, LED_WHITE_LOW);
    matrix->drawRect(3,3, width-6,height-6, LED_WHITE_VERYLOW);
    display.Invalidate();
}

void NeoLinePatternTest() {
    matrix->clear();

    // 4 levels of crossing red lines.
    matrix->drawLine(0,height/2-2, width-1,2, LED_RED_VERYLOW);
    matrix->drawLine(0,height/2-1, width-1,3, LED_RED_LOW);
    matrix->drawLine(0,height/2,   width-1,height/2, LED_RED_MEDIUM);
    matrix->drawLine(0,height/2+1, width-1,height/2+1, LED_RED_HIGH);

    // 4 levels of crossing green lines.
    matrix->drawLine(width/2-2, 0, width/2-2, height-1, LED_GREEN_VERYLOW);
    matrix->drawLine(width/2-1, 0, width/2-1, height-1, LED_GREEN_LOW);
    matrix->drawLine(width/2+0, 0, width/2+0, height-1, LED_GREEN_MEDIUM);
    matrix->drawLine(width/2+1, 0, width/2+1, height-1, LED_GREEN_HIGH);

    // Diagonal blue line.
    matrix->drawLine(0,0, width-1,height-1, LED_BLUE_HIGH);
    matrix->drawLine(0,height-1, width-1,0, LED_ORANGE_MEDIUM);
    display.Invalidate();
}

void NeoConcentricBoxes() {
    matrix->clear();
    matrix->drawRect(0,0, width,height, LED_BLUE_HIGH);
    matrix->drawRect(1,1, width-2,height-2, LED_GREEN_MEDIUM);
    matrix->fillRect(2,2, width-4,height-4, LED_RED_HIGH);
    matrix->fillRect(3,3, width-6,height-6, LED_ORANGE_MEDIUM);
    display.Invalidate();
}

void NeoConcentricCircles() {
    matrix->clear();
    matrix->drawCircle(width/2,height/2, 2, LED_RED_MEDIUM);
    matrix->drawCircle(width/2-1-min(width,height)/8, height/2-1-min(width,height)/8, min(width,height)/4, LED_BLUE_HIGH);
    matrix->drawCircle(width/2+1+min(width,height)/8, height/2+1+min(width,height)/8, min(width,height)/4-1, LED_ORANGE_MEDIUM);
    matrix->drawCircle(1,height-2, 1, LED_GREEN_LOW);
    matrix->drawCircle(width-2,1, 1, LED_GREEN_HIGH);
    if (min(width,height)>12) matrix->drawCircle(width/2-1, height/2-1, min(height/2-1,width/2-1), LED_CYAN_HIGH);
    display.Invalidate();
}

void NeoBgrGlyph(uint8_t bmp_num) { 
    static uint16_t bmx,bmy;

    NeoBgrAt(bmx, bmy, rgb8x8Glyphs[bmp_num], 8, 8);
    bmx += 8;
    if (bmx >= width) bmx = 0;
    if (!bmx) bmy += 8;
    if (bmy >= height) bmy = 0;
    display.Invalidate();
}

void NeoAllGlyphs()
{
   for (uint8_t i=0; i<=(sizeof(rgb8x8Glyphs)/sizeof(rgb8x8Glyphs[0])-1); i++)
   {
      DisplayBgrGlyph(i);
      delay(width>8?500:1500);
   }
}


void NeoDisplayReport() {
    matrix->setTextSize(1);
    // not wide enough;
    if (width<16) return;
    matrix->clear();
    // Font is 5x7, if display is too small
    // 8 can only display 1 char
    // 16 can almost display 3 chars
    // 24 can display 4 chars
    // 32 can display 5 chars
    matrix->setCursor(0, 0);
    matrix->setTextColor(matrix->Color(255,0,0));
    if (width>10) matrix->print(width/10);
    matrix->setTextColor(matrix->Color(255,128,0)); 
    matrix->print(width % 10);
    matrix->setTextColor(matrix->Color(0,255,0));
    matrix->print('x');
    // not wide enough to print 5 chars, go to next line
    if (width<25) {
      if (height==13) matrix->setCursor(6, 7);
      else if (height>=13) {
          matrix->setCursor(width-11, 8);
      } else {
          // we're not tall enough either, so we wait and display
          // the 2nd value on top.
          display.Invalidate();
          delay(2000);
          matrix->clear();
          matrix->setCursor(width-11, 0);
      }   
    }
    matrix->setTextColor(matrix->Color(0,255,128)); 
    matrix->print(height/10);
    matrix->setTextColor(matrix->Color(0,128,255));  
    matrix->print(height % 10);
    // enough room for a 2nd line
    if ((width>25 && height >14) || height>16) {
      matrix->setCursor(0, height-7);
      matrix->setTextColor(matrix->Color(0,255,255)); 
      if (width>16) matrix->print('*');
      matrix->setTextColor(matrix->Color(255,0,0)); 
      matrix->print('R');
      matrix->setTextColor(matrix->Color(0,255,0));
      matrix->print('G');
      matrix->setTextColor(matrix->Color(0,0,255)); 
      matrix->print("B");
      matrix->setTextColor(matrix->Color(255,255,0)); 
      // this one could be displayed off screen, but we don't care :)
      matrix->print("*");

      // We have a big array, great, let's assume 32x32 and add something in the middle
      if (height>24 && width>25) {
          for (uint16_t i=0; i<width; i+=8) NeoBgrAt(i, height/2-7+(i%16)/8*6, rgb8x8Glyphs[10], 8, 8);
      }
    }
    
    display.Invalidate();
}

// In a case of a tile of neomatrices, this test is helpful to make sure that the
// pixels are all in sequence (to check your wiring order and the tile options you
// gave to the constructor).
void NeoPixelOrderTest() {
    matrix->clear();
    for (uint16_t i=0; i<height; i++) {
      for (uint16_t j=0; j<width; j++) {
          matrix->drawPixel(j, i, i%3==0?LED_BLUE_HIGH:i%3==1?LED_RED_HIGH:LED_GREEN_HIGH);
          // depending on the matrix size, it's too slow to display each pixel, so
          // make the scan init faster. This will however be too fast on a small matrix.
          if (!(j%7)) matrix->show();
          yield();
      }
    }
}

#endif