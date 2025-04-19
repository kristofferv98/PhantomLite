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

// NEW: Obstacle detection functions for steering and navigation

/// Cast a ray through the world and get hit information
/// Returns distance to nearest obstacle in the specified direction
float raycast(Vector2 origin, Vector2 direction, float max_distance = 1000.0f);

/// Get steering distances around a point for navigation
/// Fills out_distances array with distances to obstacles in evenly spaced directions
/// Array must have space for num_rays entries
void get_steering_distances(Vector2 position, float* out_distances, int num_rays, float max_distance = 200.0f);

/// Check if a circle overlaps with any obstacles
bool check_circle_collision(Vector2 center, float radius);

/// Check if a rectangle overlaps with any obstacles
bool check_rect_collision(Rectangle rect);

/// Toggle debug visualization for obstacle detection
void toggle_obstacle_debug();

}  // namespace world 