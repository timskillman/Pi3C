#pragma once
#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>

#include "pico_display.hpp"
#include "Common.h"

using namespace pimoroni;

class PiType {

public:

  PiType(PicoDisplay &pico_display) {
	  init(pico_display);
  }
  
  void init(PicoDisplay &pico_display);
  void update(PicoDisplay &pico_display);

private:

  // uint16_t white;
  // uint16_t black;
  uint16_t red;
  uint16_t green;
  // uint16_t dark_grey;
  // uint16_t dark_green
  uint16_t blue;
  
  uint16_t ship[8];
  uint16_t alien1[8];

  std::vector<pt> shapes;
  std::vector<pt> aliens;
  
  uint32_t w = PicoDisplay::WIDTH;
  uint32_t h = PicoDisplay::HEIGHT;
  uint32_t sx = 10;
  uint32_t sy = 20;
  uint32_t lives = 0;
  
};
