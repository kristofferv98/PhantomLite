/// ui_adapter.cpp — implementation of UI interfaces
#include "public/ui.hpp"
#include "../features/ui/ui.hpp"
#include "../features/ui/molecules/hearts_controller.hpp"

namespace core {
namespace ui {

void update_health_display(int current_health, int max_health) {
    // Delegate to the UI module's hearts controller
    // This would typically be handled by the hearts controller in the UI module
    // For now, accessing the player's hearts controller to update the display
    ::ui::update_hearts_display(current_health, max_health);
}

void set_debug_text(const char* text, Vector2 world_position, Color color) {
    // Delegate to UI debug text display
    ::ui::show_debug_text(text, world_position, color);
}

void set_health_display_visible(bool visible) {
    // Delegate to UI hearts visibility control
    ::ui::set_hearts_visible(visible);
}

} // namespace ui
} // namespace core 