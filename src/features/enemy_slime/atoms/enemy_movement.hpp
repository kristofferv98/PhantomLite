#pragma once

#include "raylib.h"
#include "../../world/atoms/tilemap.hpp"

namespace enemy {
namespace atoms {

// Initialize the movement system
void init_movement(world::atoms::Tilemap* map);

// Update movement for all active enemies
// PERF: ~0.3-1.2ms for 10-50 enemies, depends on pathfinding complexity
void update_movement(Vector2 player_position, float delta_time);

// Clean up movement resources
void cleanup_movement();

} // namespace atoms
} // namespace enemy 