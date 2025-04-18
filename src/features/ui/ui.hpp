/// ui.hpp — Public API for game UI elements (HUD, menus, etc.)

#pragma once

namespace ui {

/// Initialize all UI systems
void init_ui();

/// Update UI systems with game time
void update_ui(float dt);

/// Render all UI elements
void render_ui();

/// Apply damage to player health
void player_take_damage(int pips);

/// Heal player health
void player_heal(int pips);

/// Get player health percentage
float get_player_health_percent();

/// Clean up UI resources
void cleanup_ui();

} // namespace ui 