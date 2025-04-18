/// ui.hpp — public interface for UI interactions
#pragma once
#include <raylib.h>

namespace core {
namespace ui {

// Health display interface
void update_health_display(int current_health, int max_health);

// Debug display - takes screen coordinates
void set_debug_text(const char* text, Vector2 screen_position, Color color = WHITE);

// UI visibility control
void set_health_display_visible(bool visible);

} // namespace ui
} // namespace core 