/// world.hpp — public interface for world interactions
#pragma once
#include <raylib.h>

namespace core {
namespace world {

// World bounds
void get_bounds(float* out_min_x, float* out_min_y, float* out_max_x, float* out_max_y);

// Camera control
void set_camera_target(const Vector2& target);

// Collision detection
bool is_position_walkable(float world_x, float world_y);

// Coordinate conversion
Vector2 screen_to_world(const Vector2& screen_pos);
Vector2 world_to_screen(const Vector2& world_pos);

} // namespace world
} // namespace core 