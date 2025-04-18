/// hearts_controller.cpp — Implementation of the Hearts UI controller
#include "hearts_controller.hpp"
#include "../atoms/hearts.hpp"
#include <raylib.h>

namespace ui {

// Static instance of Hearts
static Hearts player_hearts(10, 10); // Match player's kMaxHealthPips

void HeartsController::take_damage(int pips) {
    player_hearts.take_damage(pips);
}

void HeartsController::heal(int pips) {
    player_hearts.heal(pips);
}

bool HeartsController::is_player_alive() {
    return player_hearts.get_current_pips() > 0;
}

// Called by ui::render_ui()
void render_hearts() {
    // Create a rectangle for the hearts display in the top-left corner
    Rectangle hearts_area = { 10, 10, 200, 50 };
    player_hearts.draw_hearts(hearts_area);
}

} // namespace ui 