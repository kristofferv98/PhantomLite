/// ui.cpp — Implementation of public UI API

#include "ui.hpp"
#include "molecules/hearts_controller.hpp"

namespace ui {

// Forward declaration for function in hearts_controller.cpp
extern void render_hearts();

void init_ui() {
    // No initialization needed - HeartsController uses static methods
}

void update_ui(float dt) {
    // Nothing to update in the UI (animations could go here)
}

void render_ui() {
    // Render hearts UI
    render_hearts();
    
    // Render other UI elements as needed
}

void player_take_damage(int pips) {
    HeartsController::take_damage(pips);
}

void player_heal(int pips) {
    HeartsController::heal(pips);
}

float get_player_health_percent() {
    // Return proper percentage (alive = 1.0f, dead = 0.0f)
    return HeartsController::is_player_alive() ? 1.0f : 0.0f;
}

void cleanup_ui() {
    // No cleanup needed - all UI elements are static
}

} // namespace ui 