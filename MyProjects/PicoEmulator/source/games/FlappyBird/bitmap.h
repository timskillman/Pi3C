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

#ifndef FLAPPY_PICO_BITMAP_H
#define FLAPPY_PICO_BITMAP_H

#include "pico_display.hpp"

typedef unsigned int uint;

class Bitmap {
private:
    uint width;
    uint height;
    uint16_t *data;
public:
    Bitmap(uint width, uint height, uint16_t *data);
    /**
     * Gets the Bitmap colour at position x,y.
     */
    uint16_t get_rgb(uint x, uint y);
    /**
     * Sets the Bitmap colour at position x,y.
     */
    void set_rgb(uint x, uint y, uint16_t rgb);
    /**
     * Draws the Bitmap using a pico_graphics, starting at position x, y on the canvas and moving **down**. Pixels
     * outside the screen are ignored.
     */
    void draw(pimoroni::PicoGraphics *pico_graphics, uint x, uint y);
    /**
     * Draws the Bitmap using a pico_graphics, starting at position x, y on the canvas and moving **up**. Pixels outside
     * the screen are ignored.
     */
    void draw_reverse(pimoroni::PicoGraphics *pico_graphics, uint x, uint y);
};

#endif //FLAPPY_PICO_BITMAP_H
