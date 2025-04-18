/// movement.hpp — movement atom for player slice
#pragma once
#include <raylib.h>

namespace player {
namespace atoms {

struct MovementState {
    Vector2 position;
    bool is_moving;
    float speed;
};

// Update player position based on keyboard input
void process_movement(MovementState& state, float dt);

// Keep player within screen bounds
void constrain_to_bounds(MovementState& state, float width, float height);

// Initialize default movement state
MovementState create_movement_state(float x, float y, float speed);

} // namespace atoms
} // namespace player 