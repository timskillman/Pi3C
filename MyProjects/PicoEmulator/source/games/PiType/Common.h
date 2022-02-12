#pragma once

#include "pico_display.hpp"

using namespace pimoroni;

struct pt {
	int32_t      x;
	int32_t      y;
	uint8_t      r;
	int32_t     dx;
	int32_t     dy;
	uint16_t pen;
};

void pixel(PicoDisplay &pico_display, int x, int y, uint16_t c) {
  x *= 2;
  y *= 2;
  pico_display.frame_buffer[x + y * 240] = c;
  pico_display.frame_buffer[x + 1 + y * 240] = c;
  pico_display.frame_buffer[x + 1 + (y + 1) * 240] = c;
  pico_display.frame_buffer[x + (y + 1) * 240] = c;
}

void rect(PicoDisplay &pico_display, int x, int y, int w, int h, uint16_t c) {
  for(int rx = x; rx < x + w; rx++) {
    for(int ry = y; ry < y + h; ry++) {
      pixel(pico_display, rx, ry, c);
    }
  }
}

void sprite(PicoDisplay &pico_display, uint16_t *p, int x, int y, bool flip, uint16_t c) {
  
  if (x+16 >= 120) return;
  
  if (flip) {
      for (int ay = 0; ay < 8; ay++) {
          uint16_t sl = p[ay];
          for (int ax = 0; ax < 16 || (ax + x) < 120; ax++) {
            if ((0b1000000000000000 >> ax) & sl) {
                pixel(pico_display, ax + x, ay + y, c);
            }
          }
      }
  }
  else
  {
      for (int ay = 0; ay < 8; ay++) {
          uint16_t sl = p[ay];
          for (int ax = 0; ax < 16 || (ax + x) < 120; ax++) {
            if ((0b1 << ax) & sl) {
                pixel(pico_display, ax + x, ay + y, c);
            }
          }
      }
  }
}
