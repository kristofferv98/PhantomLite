/// enemy_renderer.hpp — Enemy rendering atom

#pragma once

#include "../enemy_slime.hpp"
#include "raylib.h"

namespace enemy {
namespace atoms {

/// Initialize the enemy renderer with textures
void init_renderer();

/// Render all active enemies
/// PERF: ~0.2-1.0ms depending on enemy count and screen size
void render_enemies();

/// Enable or disable debug visualization
void set_debug_visualization(bool enabled);

/// Toggle debug visualization mode
void toggle_debug_visualization();

/// Check if debug visualization is enabled
bool is_debug_visualization_enabled();

/// Toggle steering debug visualization (only works if main debug is enabled)
void toggle_steering_debug();

/// Check if steering debug visualization is enabled
bool is_steering_debug_enabled();

/// Clean up renderer resources
void cleanup_renderer();

} // namespace atoms
} // namespace enemy 

