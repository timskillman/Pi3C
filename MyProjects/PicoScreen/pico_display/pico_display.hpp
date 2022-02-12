#pragma once

//#include "../../drivers/st7789/st7789.hpp"
#include "pico_graphics.hpp"
#include "Pi3C.h"

namespace pimoroni {

  class PicoDisplay : public PicoGraphics {
  public:
    static const int WIDTH = 240;
    static const int HEIGHT = 135;
    static const int PORTRAIT_WIDTH  = 135;
    static const int PORTRAIT_HEIGHT = 240;
    static const uint8_t A = 12;
    static const uint8_t B = 13;
    static const uint8_t X = 14;
    static const uint8_t Y = 15;

    uint16_t *__fb;
    Pi3C pi3c;

  private:
    //ST7789 screen;
    int32_t texref = -1;
    int32_t rectRef=-1;
    int32_t dispRef=-1;

  public:
    PicoDisplay(uint16_t *buf);
    PicoDisplay(uint16_t *buf, int width, int height);

    void init(Pi3C& pi3c);
    void update(Pi3C& pi3c);
    void set_backlight(uint8_t brightness);
    void set_led(uint8_t r, uint8_t g, uint8_t b);
    bool is_pressed(uint8_t button);
    void flip();
  };

}
