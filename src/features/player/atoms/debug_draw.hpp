/// debug_draw.hpp — debug visualization atom for collision shapes
#pragma once
#include <raylib.h>
#include "collision.hpp"

namespace player {
namespace atoms {

// Draw collision shape for debugging
void draw_collision_shape(const CollisionShape& shape, Vector2 position, Color color);

// Draw all collision objects in a world
void draw_collision_world(const CollisionWorld& world, Color color);

} // namespace atoms
} // namespace player 