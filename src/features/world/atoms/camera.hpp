/// camera.hpp — camera atom for world slice
#pragma once
#include <raylib.h>

namespace world {
namespace atoms {

// Camera manages view and follows targets
class Camera {
public:
    // Initialize camera with screen dimensions
    void init(int screen_width, int screen_height);
    
    // Set target position to follow
    void set_target(const Vector2& target);
    
    // Set boundaries for camera movement
    void set_bounds(float min_x, float min_y, float max_x, float max_y);
    
    // Update camera position with smoothing
    void update(float dt);
    
    // Get camera view as Rectangle (for rendering)
    Rectangle get_view() const;
    
    // Convert screen coordinates to world coordinates
    Vector2 screen_to_world(const Vector2& screen_pos) const;
    
    // Convert world coordinates to screen coordinates
    Vector2 world_to_screen(const Vector2& world_pos) const;
    
    // Get current camera position
    const Vector2& get_position() const { return position_; }
    
private:
    Vector2 position_ = { 0, 0 };    // Current camera position (top-left corner)
    Vector2 target_ = { 0, 0 };      // Target to follow
    float smoothing_ = 5.0f;         // Lower = smoother/slower camera
    
    int screen_width_ = 0;
    int screen_height_ = 0;
    
    // Camera bounds
    float min_x_ = 0;
    float min_y_ = 0;
    float max_x_ = 0;
    float max_y_ = 0;
    bool has_bounds_ = false;
};

} // namespace atoms
} // namespace world 