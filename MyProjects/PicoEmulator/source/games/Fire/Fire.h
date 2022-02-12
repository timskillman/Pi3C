#pragma once

//From https://github.com/andreban/pico-fire
//Copyright (c) Andreban, Feb 21

#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>

#include "pico_display.hpp"
#include "../../Common/Common.h"

using namespace pimoroni;

class Fire {
public:

    Fire() {}

    Fire(PicoDisplay& pico_display) {
        init(pico_display);
    }

    void init(PicoDisplay& pico_display);
    void update(PicoDisplay& pico_display);


private:
    
    //unsigned int g_seed = 123435;

    //// Used to seed the generator.           
    //inline void fast_srand(int seed) {
    //    g_seed = seed;
    //}

    //// Compute a pseudorandom integer.
    //// Output value in range [0, 32767]
    //inline int fast_rand() {
    //    g_seed = (214013 * g_seed + 2531011);
    //    return (g_seed >> 16) & 0x7FFF;
    //}

    int posAt(int x, int y);

    uint8_t fire[pimoroni::PicoDisplay::WIDTH * pimoroni::PicoDisplay::HEIGHT];
    uint16_t pallete[36];
    int8_t wind = 0;

    bool x_pressed = false;
    bool y_pressed = false;
    bool b_pressed = false;
    bool a_pressed = false;
    bool enabled = true;
};