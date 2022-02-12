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

#include "pico_display.hpp"

#ifndef FLAPPY_PICO_SPRITES_H
#define FLAPPY_PICO_SPRITES_H

struct Sprites {
public:
    static uint16_t PLAYER_BITMAP[];
    static uint16_t PLAYER_BITMAP2[];
    static uint16_t PLAYER_BITMAP3[];
    static uint16_t PIPE_BOTTOM[];
    static uint16_t PIPE_TOP[];
};

#endif //FLAPPY_PICO_SPRITES_H
