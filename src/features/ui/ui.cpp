/// ui.cpp — Implementation of public UI API

#include "ui.hpp"
#include "molecules/hearts_controller.hpp"
#include "../player/player.hpp"

namespace ui {

// Forward declaration for function in hearts_controller.cpp
// extern void render_hearts(); // Disabled - using player::HeartsController instead

void init_ui() {
    // No UI initialization needed
}

void update_ui(float dt) {
    // Nothing to update in the UI (animations could go here)
}

void render_ui() {
    // Hearts rendering is now handled by player::HeartsController
    // render_hearts(); // Disabled
    
    // Render other UI elements as needed
}

void player_take_damage(int pips) {
    // Damage is now handled through player::take_damage
    // which updates health directly
}

void player_heal(int pips) {
    // Healing is now handled through direct health updates
}

float get_player_health_percent() {
    // Get health percent via player module
    return static_cast<float>(player::get_health()) / player::get_max_health();
}

void cleanup_ui() {
    // No UI cleanup needed
}

} // namespace ui 