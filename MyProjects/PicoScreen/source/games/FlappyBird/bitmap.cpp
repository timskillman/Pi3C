/**
 * Copyright 2021 Andre Cipriani Bandarra
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "bitmap.h"
#include <string>
//#include "pico/stdlib.h"

Bitmap::Bitmap(uint w, uint h, uint16_t *d) {
    width = w;
    height = h;
    data = d;
}

void Bitmap::draw(pimoroni::PicoGraphics *pico_graphics, uint display_x, uint display_y) {
    for (int x = 0; x < width; x++) {
        uint target_x = x + display_x;
        if (target_x >= pimoroni::PicoDisplay::WIDTH) {
            continue;
        }
        for (int y = 0; y < height; y++) {
            uint target_y = y + display_y;
            if (target_y >= pimoroni::PicoDisplay::HEIGHT) {
                break;
            }
            *pico_graphics->ptr(target_x, target_y) = get_rgb(x, y);
        }
    }
}

void Bitmap::draw_reverse(pimoroni::PicoGraphics *pico_graphics, uint display_x, uint display_y) {
    for (int x = 0; x < width; x++) {
        uint target_x = x + display_x;
        if (target_x >= pimoroni::PicoDisplay::WIDTH) {
            continue;
        }
        for (int y = 0; y < height; y++) {
            int target_y = display_y - y;
            if (target_y < 0 || target_y >= pimoroni::PicoDisplay::HEIGHT) {
                continue;
            }
            *pico_graphics->ptr(target_x, target_y) = get_rgb(x, height - y - 1);
        }
    }
}

void Bitmap::set_rgb(uint x, uint y, uint16_t rgb) {
    this->data[y * width + x] = rgb;
}

uint16_t Bitmap::get_rgb(uint x, uint y) {
    return this->data[y * width + x];
}
