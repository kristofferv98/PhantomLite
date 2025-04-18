/// hearts_controller.cpp — Implementation of hearts UI controller

#include "hearts_controller.hpp"
#include <raylib.h>

namespace ui {

// Initialize static member
Hearts HeartsController::player_hearts(12); // Start with 3 hearts (12 pips)

void HeartsController::init() {
    // Any additional initialization can go here
    // For example, loading heart textures
}

void HeartsController::update(float dt) {
    // Any animation or update logic for hearts
    // Currently, the Hearts class is simple and doesn't need updates
}

void HeartsController::render() {
    // Get the current window size for viewport calculation
    Rectangle viewport = {
        0, 0,
        static_cast<float>(GetScreenWidth()),
        static_cast<float>(GetScreenHeight())
    };
    
    // Draw the hearts based on current health
    player_hearts.draw_hearts(viewport);
}

void HeartsController::cleanup() {
    // Free any resources if needed
}

void HeartsController::take_damage(int pips) {
    player_hearts.take_damage(pips);
}

void HeartsController::heal(int pips) {
    player_hearts.heal(pips);
}

bool HeartsController::is_player_alive() {
    return player_hearts.is_alive();
}

} // namespace ui 