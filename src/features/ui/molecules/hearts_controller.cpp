/// hearts_controller.cpp — Implementation of the Hearts UI controller
#include "hearts_controller.hpp"
#include "../atoms/hearts.hpp"
#include <raylib.h>
#include <memory>

namespace ui {

// Static instance of Hearts - use unique_ptr for lazy initialization
static std::unique_ptr<Hearts> player_hearts_ptr;

void HeartsController::init(int max_pips) {
    if (!player_hearts_ptr) {
        player_hearts_ptr = std::make_unique<Hearts>(max_pips, max_pips);
    }
    // TODO: Decide if re-initialization should be allowed or reset health
}

void HeartsController::take_damage(int pips) {
    if (player_hearts_ptr) {
        player_hearts_ptr->take_damage(pips);
    }
}

void HeartsController::heal(int pips) {
    if (player_hearts_ptr) {
        player_hearts_ptr->heal(pips);
    }
}

bool HeartsController::is_player_alive() {
    if (player_hearts_ptr) {
        return player_hearts_ptr->get_current_pips() > 0;
    }
    return false; // Not initialized, assume dead
}

// Called by ui::render_ui()
void render_hearts() {
    if (player_hearts_ptr) {
        // Create a rectangle for the hearts display in the top-right corner
        Rectangle hearts_area = { 
            static_cast<float>(GetScreenWidth()) - 210.0f, // 10px from right edge
            10.0f,                                        // 10px from top edge
            200.0f, 
            50.0f 
        };
        player_hearts_ptr->draw_hearts(hearts_area);
    }
}

} // namespace ui 