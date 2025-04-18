/// camera.cpp — implementation of camera atom
#include "camera.hpp"
#include <algorithm>

namespace world {
namespace atoms {

void Camera::init(int screen_width, int screen_height) {
    screen_width_ = screen_width;
    screen_height_ = screen_height;
    position_ = { 0, 0 };
}

void Camera::set_target(const Vector2& target) {
    target_ = target;
}

void Camera::set_bounds(float min_x, float min_y, float max_x, float max_y) {
    min_x_ = min_x;
    min_y_ = min_y;
    max_x_ = max_x;
    max_y_ = max_y;
    has_bounds_ = true;
}

void Camera::update(float dt) {
    // Calculate target camera position (centered on target)
    Vector2 target_position = {
        target_.x - (screen_width_ / 2.0f),
        target_.y - (screen_height_ / 2.0f)
    };
    
    // Smoothly move towards target position
    position_.x += (target_position.x - position_.x) * smoothing_ * dt;
    position_.y += (target_position.y - position_.y) * smoothing_ * dt;
    
    // Apply bounds if set
    if (has_bounds_) {
        // Ensure camera doesn't go beyond world bounds
        position_.x = std::clamp(position_.x, min_x_, max_x_ - screen_width_);
        position_.y = std::clamp(position_.y, min_y_, max_y_ - screen_height_);
    }
}

Rectangle Camera::get_view() const {
    return {
        position_.x,
        position_.y,
        static_cast<float>(screen_width_),
        static_cast<float>(screen_height_)
    };
}

Vector2 Camera::screen_to_world(const Vector2& screen_pos) const {
    return {
        screen_pos.x + position_.x,
        screen_pos.y + position_.y
    };
}

Vector2 Camera::world_to_screen(const Vector2& world_pos) const {
    return {
        world_pos.x - position_.x,
        world_pos.y - position_.y
    };
}

} // namespace atoms
} // namespace world 