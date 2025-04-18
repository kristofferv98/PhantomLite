/// movement.cpp — implementation of movement atom
#include "movement.hpp"
#include <algorithm> // for std::clamp

namespace player {
namespace atoms {

MovementState create_movement_state(float x, float y, float speed) {
    return {
        .position = {x, y},
        .is_moving = false,
        .speed = speed
    };
}

void process_movement(MovementState& state, float dt) {
    // Reset movement flag
    state.is_moving = false;
    
    // Process keyboard input
    if (IsKeyDown(KEY_RIGHT)) {
        state.position.x += state.speed * dt;
        state.is_moving = true;
    }
    if (IsKeyDown(KEY_LEFT)) {
        state.position.x -= state.speed * dt;
        state.is_moving = true;
    }
    if (IsKeyDown(KEY_DOWN)) {
        state.position.y += state.speed * dt;
        state.is_moving = true;
    }
    if (IsKeyDown(KEY_UP)) {
        state.position.y -= state.speed * dt;
        state.is_moving = true;
    }
}

void constrain_to_bounds(MovementState& state, float width, float height) {
    state.position.x = std::clamp(state.position.x, width / 2, GetScreenWidth() - width / 2);
    state.position.y = std::clamp(state.position.y, height / 2, GetScreenHeight() - height / 2);
}

} // namespace atoms
} // namespace player 