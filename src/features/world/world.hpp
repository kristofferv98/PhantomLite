/// world.hpp — public API for World slice (tilemap, camera)
#pragma once
#include <raylib.h>

namespace world {

void init();
void update(float dt);
void render();
void cleanup();

// Set player position for camera to follow
void set_camera_target(const Vector2& target);

// Check if a position is walkable
bool is_walkable(float world_x, float world_y);

// Get world bounds for collision checking
void get_world_bounds(float* out_min_x, float* out_min_y, float* out_max_x, float* out_max_y);

// Convert screen coordinates to world coordinates
Vector2 screen_to_world(const Vector2& screen_pos);

// Convert world coordinates to screen coordinates
Vector2 world_to_screen(const Vector2& world_pos);

}  // namespace world 