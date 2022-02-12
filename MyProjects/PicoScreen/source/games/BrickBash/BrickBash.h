#pragma once

#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>

#include "pico_display.hpp"

using namespace pimoroni;

class BrickBash {
public:

  BrickBash() {}
  
  BrickBash(PicoDisplay &pico_display) {
    init(pico_display);
  }
  
  void init(PicoDisplay &pico_display);
  void update(PicoDisplay &pico_display);

private:
  
  struct pt {
    int32_t      x;
    int32_t      y;
    uint8_t      r;
    int32_t     dx;
    int32_t     dy;
    uint16_t pen;
  };

  uint32_t w = pimoroni::PicoDisplay::WIDTH;
  uint32_t h = pimoroni::PicoDisplay::HEIGHT;
  uint32_t sx = 10;
  uint32_t sy = 20;
  uint32_t score = 0;
  uint32_t lives = 3;
  uint32_t level = 0;
  bool start = true;
  int maxlevels = 5;
  
  uint16_t bricks[8][8];
  Pen brick_cols[8];
  Pen bat_cols[5];
  uint16_t brickx = 0;
  uint16_t bricky = 4;
  
  pt ball;
  pt bat;
};

