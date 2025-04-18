/// ui_adapter.cpp — implementation of UI interfaces
#include "public/ui.hpp"
#include "../features/ui/ui.hpp"
#include "../features/player/molecules/hearts_controller.hpp"
#include <raylib.h>

namespace core {
namespace ui {

void update_health_display(int current_health, int max_health) {
    // Delegate to the player module's hearts controller
    player::HeartsController::update(current_health, max_health);
}

void set_debug_text(const char* text, Vector2 screen_position, Color color) {
    // Draw debug text at screen position
    // This is now using screen coordinates directly, not world coordinates
    DrawText(text, (int)screen_position.x, (int)screen_position.y, 20, color);
}

void set_health_display_visible(bool visible) {
    // Delegate to UI hearts visibility control
    // This would call the appropriate UI function when implemented
}

} // namespace ui
} // namespace core 