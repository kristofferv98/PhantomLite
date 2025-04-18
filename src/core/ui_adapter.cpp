/// ui_adapter.cpp — implementation of UI interfaces
#include "public/ui.hpp"
#include "../features/ui/ui.hpp"

namespace core {
namespace ui {

void update_health_display(int current_health, int max_health) {
    // This would delegate to the UI module's hearts controller
    // For now, it's a stub that will need to be implemented
}

void set_debug_text(const char* text, Vector2 world_position, Color color) {
    // This would delegate to UI debug text display
    // For now, it's a stub that will need to be implemented
}

void set_health_display_visible(bool visible) {
    // This would delegate to UI hearts visibility control
    // For now, it's a stub that will need to be implemented
}

} // namespace ui
} // namespace core 