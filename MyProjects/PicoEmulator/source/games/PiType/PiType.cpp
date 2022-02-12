#include "PiType.h"

void PiType::init(PicoDisplay &pico_display)
{
  
  ship[0] = 0b1100000000000000;
  ship[1] = 0b1110000001111000;
  ship[2] = 0b0111011111000100;
  ship[3] = 0b1111111111111111;
  ship[4] = 0b0111111000011100;
  ship[5] = 0b0011100000000000;
  ship[6] = 0b0110000000000000;
  ship[7] = 0b0000000000000000;

  alien1[0] = 0b0001111110000000;
  alien1[1] = 0b1110000001111000;
  alien1[2] = 0b0000011111000110;
  alien1[3] = 0b1111111111111111;
  alien1[4] = 0b0000011111000110;
  alien1[5] = 0b1110000001111000;
  alien1[6] = 0b0001111110000000;
  alien1[7] = 0b0000000000000000;
  
  // white = pico_display.create_pen(255, 255, 255);
  // black = pico_display.create_pen(0, 0, 0);
  red = pico_display.create_pen(255, 0, 0);
  green = pico_display.create_pen(0, 255, 0);
  // dark_grey = pico_display.create_pen(20, 40, 60);
  // dark_green = pico_display.create_pen(10, 100, 10);
  blue = pico_display.create_pen(0, 0, 255);
  
  shapes.resize(0);
  for(int i = 0; i < 100; i++) {
    pt shape;
    shape.x = rand() % 240;
    shape.y = rand() % 135;
    shape.r = (rand() % 10) + 3;
    shape.dx = -rand() % 5 - 1; //float(rand() % 255) / 128.0f;
    //shape.dy = float(rand() % 255) / 128.0f;
    uint8_t grey = rand() % 255;
    shape.pen = pico_display.create_pen(grey, grey, grey);
    shapes.push_back(shape);
  }
  
  aliens.resize(0);
  for(int i = 0; i < 5; i++) {
    pt alien;
    alien.x = 120+(rand() % 120);
    alien.y = rand() % 54;
    alien.r = (rand() % 10) + 3;
    alien.dx = -rand() % 3 - 1; //float(rand() % 255) / 128.0f;
    //shape.dy = float(rand() % 255) / 128.0f;
    alien.pen = pico_display.create_pen(rand() % 255, rand() % 255, rand() % 255);
    aliens.push_back(alien);
  }
}

void PiType::update(PicoDisplay &pico_display)
{
    for(auto &shape : shapes) {
      shape.x += shape.dx;
      //shape.y += shape.dy;
      if(shape.x < 0) shape.x += 240;
      //if(shape.x >= pico_display.bounds.w) shape.dx *= -1;
      //if(shape.y < 0) shape.dy *= -1;
      //if(shape.y >= pico_display.bounds.h) shape.dy *= -1;

      pico_display.set_pen(shape.pen);
      pico_display.line(Point(shape.x, shape.y), Point(shape.x + shape.r, shape.y));
      //pico_display.circle(Point(shape.x, shape.y), shape.r);
    }
    
    pico_display.set_pen(255, 255, 255);
    sprite(pico_display, ship, sx, sy, true, red);
      
    for(auto &alien : aliens) {
      alien.x += alien.dx;
      //shape.y += shape.dy;
      if(alien.x < 0) { 
        alien.x = 120+(rand() % 120);
        alien.y = rand() % 54;
      }
      
      pico_display.set_pen(alien.pen);
      sprite(pico_display, alien1, alien.x, alien.y, true, blue);
      //pico_display.circle(Point(shape.x, shape.y), shape.r);
    }
    
    if(pico_display.is_pressed(pico_display.A)) {
      //pico_display.rectangle(Point(0, 0), Point(18, 18));
      if (sy>0) sy--;
      //sprite(tick, 5, 5, true, green);
    }else{
      //sprite(arrow, 10 + bounce, 10, true, white);
    }

    if(pico_display.is_pressed(pico_display.B)) {
      if (sy<56) sy++;
      //pico_display.rectangle(Point(0, 49), Point(18, 18));
      //sprite(tick, 5, 54, true, green);
    }else{
      //sprite(arrow, 10 - bounce, 50, true, white);
    }
}
