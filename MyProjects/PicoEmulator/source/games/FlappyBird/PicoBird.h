#pragma once

#include <string.h>
#include <math.h>
#include <vector>
#include <cstdlib>

#include "pico_display.hpp"
#include "../../Common/Common.h"
#include "sprites.h"
#include "bitmap.h"

using namespace pimoroni;

const uint8_t BIRD_SIZE = 16;
const uint8_t BOARD_WIDTH = pimoroni::PicoDisplay::WIDTH;
const uint8_t BOARD_HEIGHT = pimoroni::PicoDisplay::HEIGHT;

/**
 * The Player.
 */
class Player {
public:
    pimoroni::Rect bounds = pimoroni::Rect(32, 65, BIRD_SIZE, BIRD_SIZE);
    Bitmap* bitmap[9] = {
            new Bitmap(16, 16, Sprites::PLAYER_BITMAP),
            new Bitmap(16, 16, Sprites::PLAYER_BITMAP),
            new Bitmap(16, 16, Sprites::PLAYER_BITMAP),
            new Bitmap(16, 16, Sprites::PLAYER_BITMAP2),
            new Bitmap(16, 16, Sprites::PLAYER_BITMAP2),
            new Bitmap(16, 16, Sprites::PLAYER_BITMAP2),
            new Bitmap(16, 16, Sprites::PLAYER_BITMAP3),
            new Bitmap(16, 16, Sprites::PLAYER_BITMAP3),
            new Bitmap(16, 16, Sprites::PLAYER_BITMAP3),
    };
    int current_image = 0;

    Player() {
    }

    void reset() {
        bounds.y = 65;
    }

    void update(PicoDisplay& pico_display) {
        int speed = pico_display.is_pressed(pimoroni::PicoDisplay::Y) ? -1 : 1;
        bounds.y += speed;
        current_image++;
    }

    void render(PicoDisplay& pico_display) {
        bitmap[current_image % 9]->draw(&pico_display, this->bounds.x, this->bounds.y);
    }
};

/**
 * The obstacle walls.
 */
class Obstacle {
public:
    pimoroni::Rect top_wall;
    pimoroni::Rect bottom_wall;
    Bitmap* bottom_image = new Bitmap(32, 135, Sprites::PIPE_BOTTOM);
    Bitmap* top_image = new Bitmap(32, 135, Sprites::PIPE_TOP);
    int start_x;

    Obstacle(int start_x) {
        this->start_x = start_x;
        reset();
    }

    void reset() {
        int rand = fast_rand();
        uint8_t y_gap = (rand % 64) + 32;
        uint8_t top_wall_height = (rand % (BOARD_HEIGHT - y_gap)) + 1;
        uint8_t bottom_wall_height = BOARD_HEIGHT - top_wall_height - y_gap;
        top_wall = pimoroni::Rect(start_x, 0, 32, top_wall_height);
        bottom_wall = pimoroni::Rect(
            start_x, BOARD_HEIGHT - bottom_wall_height, 32, bottom_wall_height);
    }

    void update() {
        top_wall.x -= 1;
        bottom_wall.x -= 1;
        if (top_wall.x + top_wall.w < 0 || bottom_wall.x + bottom_wall.w < 0) {
            top_wall.x = BOARD_WIDTH;
            bottom_wall.x = BOARD_WIDTH;
        }
    }

    void render(PicoDisplay& pico_display) {
        top_image->draw_reverse(&pico_display, top_wall.x, top_wall.y + top_wall.h);
        bottom_image->draw(&pico_display, bottom_wall.x, bottom_wall.y);
    }
};

/**
* The game state.
*/
enum GameState {
    MENU,
    GAME_PLAY,
    GAME_OVER
};


#define NUM_OBSTACLES 3

class PicoBird {
public:

    PicoBird() {}

    PicoBird(PicoDisplay& pico_display) {
        init(pico_display);
    }

    void init(PicoDisplay& pico_display);
    void update(PicoDisplay& pico_display);

private:

    Obstacle* obstacles[NUM_OBSTACLES] = {
            new Obstacle(BOARD_WIDTH),
            new Obstacle(BOARD_WIDTH + BOARD_WIDTH / 3),
            new Obstacle(BOARD_WIDTH + (BOARD_WIDTH / 3 * 2))
    };

    GameState gameState = GameState::MENU;
    Player* player = new Player();
    uint32_t score = 0;

    void resetGame();
    void gameplay(PicoDisplay& pico_display);
    void menu(PicoDisplay& pico_display);
    void gameover(PicoDisplay& pico_display);

};



