#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>

#include "pico_display.hpp"
//#include "Games/BrickBash/BrickBash.h"
//#include "Games/Fire/Fire.h"
//#include "Games/FlappyBird/PicoBird.h"
#include "Games/VectorMap/Vector.h"

using namespace pimoroni;

uint16_t buffer[PicoDisplay::WIDTH * PicoDisplay::HEIGHT];
PicoDisplay pico_display(buffer);

int main(int argc, char* argv[]) {

  Pi3C pi3c = Pi3C("PicoWindow", 800, 600);

  pico_display.init(pi3c);
  pico_display.set_backlight(100);
  
  uint32_t i = 0;
  
  
  //BrickBash brickbash(pico_display);
  //Fire fire(pico_display);
  //PicoBird picobird(pico_display);
  VectorMap vectormap(pico_display);

  while(pi3c.is_running()) {

    float led_step = fmod(i / 20.0f, M_PI * 2.0f);
    int r = (sin(led_step) * 25.0f) + 25.0f;
    pico_display.set_led(r, r / 1.2f, r);

    //brickbash.update(pico_display);
    //fire.update(pico_display);
    //picobird.update(pico_display);
    vectormap.update(pico_display);

    pico_display.update(pi3c);

    i++;
  }

  return 0;
}
