/// player.hpp — public API for Player slice (movement, combat)
#pragma once
namespace player {

enum class PlayerState {
    IDLE,
    WALKING,
    ATTACKING
};

void init();
void update(float dt);
void render();
void cleanup(); // New function to unload textures

// Animation control
void set_animation(PlayerState state);

}  // namespace player 