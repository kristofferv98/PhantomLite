/// world_adapter.cpp — implementation of world interfaces
#include "public/world.hpp"
#include "../features/world/world.hpp"

namespace core {
namespace world {

void get_bounds(float* out_min_x, float* out_min_y, float* out_max_x, float* out_max_y) {
    // Delegate to world module
    ::world::get_world_bounds(out_min_x, out_min_y, out_max_x, out_max_y);
}

void set_camera_target(const Vector2& target) {
    // Delegate to world module
    ::world::set_camera_target(target);
}

bool is_position_walkable(float world_x, float world_y) {
    // Delegate to world module
    return ::world::is_walkable(world_x, world_y);
}

Vector2 screen_to_world(const Vector2& screen_pos) {
    // Delegate to world module
    return ::world::screen_to_world(screen_pos);
}

Vector2 world_to_screen(const Vector2& world_pos) {
    // Delegate to world module
    return ::world::world_to_screen(world_pos);
}

} // namespace world
} // namespace core 