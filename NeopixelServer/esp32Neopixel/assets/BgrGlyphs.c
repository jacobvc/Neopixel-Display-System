#include <stdio.h>
#include <stdint.h>
#include "BgrGlyphs.h"

long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void NeoBgrAt(const uint16_t *bitmap, int16_t w, int16_t h, int index) {
    printf("\n  // %d: \n  { ", index);
    for (uint16_t pixel=0; pixel<w*h; pixel++) {
      uint8_t r,g,b;
      uint16_t color = bitmap[pixel];

      if (pixel % 8 == 0) {
        printf("\n    ");
      }
      //Serial.print(color, HEX);
      b = (color & 0xF00) >> 8;
      g = (color & 0x0F0) >> 4;
      r = color & 0x00F;

      // expand from 4/4/4 bits per color to 5/6/5
      b = map(b, 0, 15, 0, 31);
      g = map(g, 0, 15, 0, 63);
      r = map(r, 0, 15, 0, 31);
      printf("0x%04x, ", (r << 11) + (g << 5) + b);
    }
    printf("\n  },\n");
}

char beginning[] = "#if defined(__AVR__)\n\
#include <avr/pgmspace.h>\n\
#elif defined(__PIC32MX__)\n\
#define PROGMEM\n\
#elif defined(__arm__)\n\
#define PROGMEM\n\
#endif\n\
\n\
const uint16_t PROGMEM\n\
  rgb8x8Glyphs[][64] = {";

int main(int argc, char *argv[])
{
  printf(beginning);
  for (int i = 0; i <= 10; ++i) 
    NeoBgrAt(bgr8x8Glyphs[i], 8, 8, i);
  printf("};");
}