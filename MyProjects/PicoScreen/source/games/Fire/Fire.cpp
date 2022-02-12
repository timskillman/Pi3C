#include "Fire.h"

//From https://github.com/andreban/pico-fire
//Copyright (c) Andreban, Feb 21

void Fire::init(PicoDisplay& pico_display)
{
    pallete[0] = pico_display.create_pen(0x07, 0x07, 0x07);
    pallete[1] = pico_display.create_pen(0x1f, 0x07, 0x07);
    pallete[2] = pico_display.create_pen(0x2f, 0x0f, 0x07);
    pallete[3] = pico_display.create_pen(0x47, 0x0f, 0x07);
    pallete[4] = pico_display.create_pen(0x57, 0x17, 0x07); 
    pallete[5] = pico_display.create_pen(0x67, 0x1f, 0x07);    
    pallete[6] = pico_display.create_pen(0x77, 0x1f, 0x07);    
    pallete[7] = pico_display.create_pen(0x8f, 0x27, 0x07);
    pallete[8] = pico_display.create_pen(0x9f, 0x2f, 0x07);    
    pallete[9] = pico_display.create_pen(0xaf, 0x3f, 0x07);
    pallete[10] = pico_display.create_pen(0xbf, 0x47, 0x07);    
    pallete[11] = pico_display.create_pen(0xc7, 0x47, 0x07);
    pallete[12] = pico_display.create_pen(0xDF, 0x4F, 0x07);    
    pallete[13] = pico_display.create_pen(0xDF, 0x57, 0x07);
    pallete[14] = pico_display.create_pen(0xDF, 0x57, 0x07);    
    pallete[15] = pico_display.create_pen(0xD7, 0x5F, 0x07);
    pallete[16] = pico_display.create_pen(0xD7, 0x67, 0x0F);    
    pallete[17] = pico_display.create_pen(0xcf, 0x6f, 0x0f);
    pallete[18] = pico_display.create_pen(0xcf, 0x77, 0x0f);    
    pallete[19] = pico_display.create_pen(0xcf, 0x7f, 0x0f);
    pallete[20] = pico_display.create_pen(0xCF, 0x87, 0x17);    
    pallete[21] = pico_display.create_pen(0xC7, 0x87, 0x17);
    pallete[22] = pico_display.create_pen(0xC7, 0x8F, 0x17);    
    pallete[23] = pico_display.create_pen(0xC7, 0x97, 0x1F);
    pallete[24] = pico_display.create_pen(0xBF, 0x9F, 0x1F);    
    pallete[25] = pico_display.create_pen(0xBF, 0x9F, 0x1F);
    pallete[26] = pico_display.create_pen(0xBF, 0xA7, 0x27);    
    pallete[27] = pico_display.create_pen(0xBF, 0xA7, 0x27);
    pallete[28] = pico_display.create_pen(0xBF, 0xAF, 0x2F);    
    pallete[29] = pico_display.create_pen(0xB7, 0xAF, 0x2F);
    pallete[30] = pico_display.create_pen(0xB7, 0xB7, 0x2F);    
    pallete[31] = pico_display.create_pen(0xB7, 0xB7, 0x37);
    pallete[32] = pico_display.create_pen(0xCF, 0xCF, 0x6F);    
    pallete[33] = pico_display.create_pen(0xDF, 0xDF, 0x9F);
    pallete[34] = pico_display.create_pen(0xEF, 0xEF, 0xC7);    
    pallete[35] = pico_display.create_pen(0xFF, 0xFF, 0xFF);


}

int Fire::posAt(int x, int y) {
    return y * pimoroni::PicoDisplay::WIDTH + x;
}

void Fire::update(PicoDisplay& pico_display) 
{
    for (int y = 0; y < pimoroni::PicoDisplay::HEIGHT; y++) {
        int row = y * pimoroni::PicoDisplay::WIDTH;
        int next_row = y == 0 ? 0 : (y - 1) * pimoroni::PicoDisplay::WIDTH;
        for (int x = 0; x < pimoroni::PicoDisplay::WIDTH; x++) {
            uint8_t color = fire[row + x];
            uint16_t pen = pallete[color];
            *pico_display.ptr(x, y) = pen;

            if (y > 0) {
                int new_x = x;
                int rand = fast_rand() % 3;
                new_x = (new_x + rand - 1 + wind);
                if (new_x >= pimoroni::PicoDisplay::WIDTH) {
                    new_x = new_x - pimoroni::PicoDisplay::WIDTH;
                }
                else if (new_x < 0) {
                    new_x = new_x + pimoroni::PicoDisplay::WIDTH;
                }
                color = color > 0 ? color - (rand & 1) : 0;
                fire[next_row + new_x] = color;
            }
        }
    }

    if (!y_pressed && pico_display.is_pressed(pimoroni::PicoDisplay::Y)) {
        wind++;
    }
    y_pressed = pico_display.is_pressed(pimoroni::PicoDisplay::Y);

    if (!b_pressed && pico_display.is_pressed(pimoroni::PicoDisplay::B)) {
        wind--;
    }
    b_pressed = pico_display.is_pressed(pimoroni::PicoDisplay::B);

    if (!a_pressed && pico_display.is_pressed(pimoroni::PicoDisplay::A)) {
        uint8_t color_index = enabled ? 0 : 35;
        for (int i = 0; i < pimoroni::PicoDisplay::WIDTH; i++) {
            uint32_t pos = posAt(i, pimoroni::PicoDisplay::HEIGHT - 1);
            fire[pos] = color_index;
        }
        enabled = !enabled;
    }
    a_pressed = pico_display.is_pressed(pimoroni::PicoDisplay::A);

    //if (!x_pressed && pico_display.is_pressed(pimoroni::PicoDisplay::X)) {
    //    show_fps = !show_fps;
    //}
    //x_pressed = pico_display.is_pressed(pimoroni::PicoDisplay::X);
}