/// movement.cpp — implementation of movement atom
#include "movement.hpp"
#include <algorithm> // for std::clamp
#include <cmath> // for sqrtf

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
    
    // Calculate movement vector based on input
    Vector2 movement = {0.0f, 0.0f};
    
    if (IsKeyDown(KEY_RIGHT)) {
        movement.x += 1.0f;
    }
    if (IsKeyDown(KEY_LEFT)) {
        movement.x -= 1.0f;
    }
    if (IsKeyDown(KEY_DOWN)) {
        movement.y += 1.0f;
    }
    if (IsKeyDown(KEY_UP)) {
        movement.y -= 1.0f;
    }
    
    // Check if we're moving
    state.is_moving = (movement.x != 0.0f || movement.y != 0.0f);
    
    // If moving, normalize the movement vector for consistent speed in all directions
    if (state.is_moving) {
        // Calculate the magnitude of the movement vector
        float magnitude = sqrtf(movement.x * movement.x + movement.y * movement.y);
        
        // Normalize only if magnitude is not zero (which should never happen, but just in case)
        if (magnitude > 0.0f) {
            movement.x /= magnitude;
            movement.y /= magnitude;
            
            // Apply normalized movement with consistent speed
            state.position.x += movement.x * state.speed * dt;
            state.position.y += movement.y * state.speed * dt;
        }
    }
}

void constrain_to_bounds(MovementState& state, float width, float height) {
    state.position.x = std::clamp(state.position.x, width / 2, GetScreenWidth() - width / 2);
    state.position.y = std::clamp(state.position.y, height / 2, GetScreenHeight() - height / 2);
}

} // namespace atoms
} // namespace player 