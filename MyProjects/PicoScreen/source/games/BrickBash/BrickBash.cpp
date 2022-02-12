#include "BrickBash.h"

void BrickBash::init(PicoDisplay &pico_display)
{

  ball.x = (rand() % 200)<<16;
  ball.y = (rand() % 120)<<16;
  ball.dx = (1<<15);
  ball.dy = (int32_t)(-1.0f * 65536.f);
  ball.r = 5;
  
  bat.r = 30;
  bat.x = 120-bat.r/2;
  bat.y = 120;

  bricks[0][0] = 0b0011111101111110;
  bricks[0][1] = 0b0111111010111111;
  bricks[0][2] = 0b0110110101011011;
  bricks[0][3] = 0b0111001010100111;
  bricks[0][4] = 0b0111110101011111;
  bricks[0][5] = 0b0111111010111111;
  bricks[0][6] = 0b0011110101011110;
  bricks[0][7] = 0b0000000000000000;
  
  bricks[1][0] = 0b0111111111111111;
  bricks[1][1] = 0b0111001001001111;
  bricks[1][2] = 0b0111110000111111;
  bricks[1][3] = 0b0111001111001111;
  bricks[1][4] = 0b0110011111100111;
  bricks[1][5] = 0b0111001111001111;
  bricks[1][6] = 0b0111110000111111;
  bricks[1][7] = 0b0111111111111111;
  
  bricks[2][0] = 0b0011111111111110;
  bricks[2][1] = 0b0111001111100111;
  bricks[2][2] = 0b0110110111011011;
  bricks[2][3] = 0b0111001111100111;
  bricks[2][4] = 0b0111111111111111;
  bricks[2][5] = 0b0111001111100111;
  bricks[2][6] = 0b0111110000011111;
  bricks[2][7] = 0b0011111111111110;
  
  bricks[3][0] = 0b0111111111111111;
  bricks[3][1] = 0b0111111111111111;
  bricks[3][2] = 0b0111111111111111;
  bricks[3][3] = 0b0111111111111111;
  bricks[3][4] = 0b0111111111111111;
  bricks[3][5] = 0b0111111111111111;
  bricks[3][6] = 0b0111111111111111;
  bricks[3][7] = 0b0111111111111111;
  
  bricks[4][0] = 0b0111111000111111;
  bricks[4][1] = 0b0111111000111111;
  bricks[4][2] = 0b0111000000000111;
  bricks[4][3] = 0b0111000000000111;
  bricks[4][4] = 0b0111111000111111;
  bricks[4][5] = 0b0111111000111111;
  bricks[4][6] = 0b0111111000111111;
  bricks[4][7] = 0b0111111111111111;

  bricks[5][0] = 0b0011101111111000;
  bricks[5][1] = 0b0101011111100111;
  bricks[5][2] = 0b0110111111011111;
  bricks[5][3] = 0b0111001110111111;
  bricks[5][4] = 0b0111110001111111;
  bricks[5][5] = 0b0111101110011111;
  bricks[5][6] = 0b0111011111100111;
  bricks[5][7] = 0b0111101111111000;

  bricks[6][0] = 0b0001111111111100;
  bricks[6][1] = 0b0011001111100110;
  bricks[6][2] = 0b0110110111011011;
  bricks[6][3] = 0b0111001111100111;
  bricks[6][4] = 0b0111110000011111;
  bricks[6][5] = 0b0111001111100111;
  bricks[6][6] = 0b0011110000011110;
  bricks[6][7] = 0b0001111111111100;
  
  bricks[7][0] = 0b0111111111111111;
  bricks[7][1] = 0b0101010101010101;
  bricks[7][2] = 0b0110101010101011;
  bricks[7][3] = 0b0101010101010101;
  bricks[7][4] = 0b0110101010101011;
  bricks[7][5] = 0b0101010101010101;
  bricks[7][6] = 0b0110101010101011;
  bricks[7][7] = 0b0111111111111111;
  
  brick_cols[0] = pico_display.create_pen(255, 0, 0);
  brick_cols[1] = pico_display.create_pen(255, 128, 0);
  brick_cols[2] = pico_display.create_pen(255, 255, 0);
  brick_cols[3] = pico_display.create_pen(0, 255, 0);
  brick_cols[4] = pico_display.create_pen(0, 128, 255);
  brick_cols[5] = pico_display.create_pen(0, 0, 255);
  brick_cols[6] = pico_display.create_pen(128, 0, 255);
  brick_cols[7] = pico_display.create_pen(255, 0, 128);
  
  bat_cols[0] = pico_display.create_pen(0, 0, 255);
  bat_cols[1] = pico_display.create_pen(0, 255, 255);
  bat_cols[2] = pico_display.create_pen(255, 255, 255);
  bat_cols[3] = pico_display.create_pen(0, 255, 255);
  bat_cols[4] = pico_display.create_pen(0, 0, 255);
  
  lives = 3;
  score = 0;
  level = 0;
  start = true;
  
}

void BrickBash::update(PicoDisplay &pico_display)
{
    //Draw bat
    for (int i=0; i<5; i++) {
        pico_display.set_pen(bat_cols[i]);
        pico_display.line(Point(bat.x, bat.y + i - 1), Point(bat.x + bat.r, bat.y + i - 1));
    }
    
    //Draw ball
    pico_display.set_pen(128, 255, 128);
    pico_display.circle(Point(ball.x>>16, ball.y>>16), ball.r);
    
    //Draw bricks
    for (int i=0; i<8; i++)
    {
      if (bricks[level][i] != 0) {
        for (int32_t xb=0; xb<16; xb++) {
          if (bricks[level][i] & (1<<xb)) {
            pico_display.set_pen(brick_cols[i]);
            pico_display.rectangle(Rect(brickx + xb*16, bricky+i*8, 14, 8));
          }
        }
      }
    }
    
    //Check game status
    if (lives==0) {
        pico_display.set_pen(255, 255, 255);
        pico_display.text("GAME OVER!",Point(60,60),500,3);
        if (pico_display.is_pressed(pico_display.X)) {
            init(pico_display);
        }
    }
    else
    {
        if (start) {
            ball.x = (bat.x + 10)<<16;
            ball.y = (bat.y - 4)<<16;
            if (pico_display.is_pressed(pico_display.X)) {
                start = false;
                while (!pico_display.is_pressed(pico_display.X));
            }
        }
        else 
        {
            ball.x += ball.dx; // * speed;
            ball.y += ball.dy; // * speed;
        }
        pico_display.set_pen(128, 255, 128);
        for (int i=0; i<lives-1; i++) {
            pico_display.circle(Point(w-i*10-5, 130), ball.r);
        }
    }
    
    //Ball hit walls?
    int32_t bx = ball.x >> 16;
    int32_t by = ball.y >> 16;
    if(bx >= pico_display.bounds.w || ball.x < 0) ball.dx *= -1;
    if(ball.y < 0) ball.dy *= -1;
    
    //Ball hit ground?
    if (by >= pico_display.bounds.h) {
        if (lives>0) lives--;
        start = true;
    }

    //Ball hit bat?
    if(by == (bat.y-3) && (bx > bat.x && bx<(bat.x+bat.r))) {
      ball.dy *= -1;
      ball.dx = ((bx-(bat.x+bat.r/2))<<16) / 16;
    }
    
    //Ball hit bricks? ...
    bool finished = true;
    for (int i=0; i<8; i++)
    {
      if (bricks[level][i] != 0) {
        if (by >= bricky+8*i && by < bricky+8*(i+1)) {
          int32_t xb = (bx-brickx) >> 4; //16
          if (xb>=0 && xb<16) {
            if (bricks[level][i] & (1<<xb)) {
              bricks[level][i] = bricks[level][i] ^ (1<<xb);
              ball.dy *= -1;
              score += (8-i)*20;
            }
          }
        }
        finished = false;
      }
    }
    
    if (finished) {
      level = (level+1) % maxlevels;
      lives++;
      start = true;
    }
    
    //Check controllers
    if(pico_display.is_pressed(pico_display.Y) && bat.x<(240-bat.r)) {
      bat.x+=2;
    }
    
    if(pico_display.is_pressed(pico_display.B) && bat.x>0) {
      bat.x-=2;
    }
    
    //Display score
    pico_display.set_pen(255, 255, 255);
    std::string sc = std::to_string(score);
    pico_display.text("SCORE:"+sc,Point(0,122),0,2);
}
