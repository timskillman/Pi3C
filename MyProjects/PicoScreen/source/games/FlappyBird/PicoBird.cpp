#include "PicoBird.h"

void PicoBird::init(PicoDisplay& pico_display)
{

}

void PicoBird::update(PicoDisplay& pico_display)
{
    switch (gameState) {
    case GameState::MENU:
        menu(pico_display);
        break;
    case GameState::GAME_PLAY:
        gameplay(pico_display);
        break;
    case GameState::GAME_OVER:
        gameover(pico_display);
        break;
    }
}

void PicoBird::resetGame() {
    player->reset();
    for (int i = 0; i < NUM_OBSTACLES; i++) {
        obstacles[i]->reset();
    }
    score = 0;
}

void PicoBird::gameplay(PicoDisplay& pico_display) {
    // Clear screen
    pico_display.set_pen(34, 3, 77);
    pico_display.clear();

    player->update(pico_display);
    for (int i = 0; i < NUM_OBSTACLES; i++) {
        obstacles[i]->update();
    }

    if (player->bounds.y < 0 ||
        player->bounds.y >= BOARD_HEIGHT - 15) {
        gameState = GameState::GAME_OVER;
    }

    for (int i = 0; i < NUM_OBSTACLES; i++) {
        if (player->bounds.intersects(obstacles[i]->top_wall) ||
            player->bounds.intersects(obstacles[i]->bottom_wall)) {
            gameState = GameState::GAME_OVER;
            break;
        }
    }

    // render game
    for (int i = 0; i < NUM_OBSTACLES; i++) {
        obstacles[i]->render(pico_display);
    }
    player->render(pico_display);
    score++;
    pico_display.set_pen(221, 221, 221);
    pico_display.text("Score: " + std::to_string(score), pimoroni::Point(180, 0), 0);
}

/**
 * Renders the menu state.
 */
void PicoBird::menu(PicoDisplay& pico_display) {
    pico_display.set_pen(221, 221, 221);
    pico_display.text("Pico Bird", pimoroni::Point(8, 15), 240, 5);
    pico_display.text("Press Any Button", pimoroni::Point(35, 70), 240, 2);
    pico_display.text("Sprites by TheWitch7971", pimoroni::Point(5, 115), 200, 1);
    pico_display.text("Code by andreban", pimoroni::Point(5, 125), 200, 1);
    if (pico_display.is_pressed(pimoroni::PicoDisplay::A) ||
        pico_display.is_pressed(pimoroni::PicoDisplay::B) ||
        pico_display.is_pressed(pimoroni::PicoDisplay::X) ||
        pico_display.is_pressed(pimoroni::PicoDisplay::Y)) {
        resetGame();
        gameState = GameState::GAME_PLAY;
    }
}

/**
 * Renders the gameover state.
 */
void PicoBird::gameover(PicoDisplay& pico_display) {
    pico_display.set_pen(221, 221, 221);
    pico_display.text("Game Over", pimoroni::Point(100, 55), 0);
    if (pico_display.is_pressed(pimoroni::PicoDisplay::A) ||
        pico_display.is_pressed(pimoroni::PicoDisplay::B) ||
        pico_display.is_pressed(pimoroni::PicoDisplay::X) ||
        pico_display.is_pressed(pimoroni::PicoDisplay::Y)) {
        resetGame();
        gameState = GameState::GAME_PLAY;
    }
}