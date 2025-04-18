/// player.hpp — public API for Player slice (movement, combat)
#pragma once
#include "raylib.h"

namespace player {

enum class PlayerState {
    IDLE,
    WALKING,
    ATTACKING
};

// Initialize the player slice
void init(float start_x, float start_y);

// Update the player (movement, animation, etc)
void update(float dt);

// Render the player
void render();

// Cleanup player resources
void cleanup();

// Get the player's current position
Vector2 get_position();

// Apply damage to the player and knockback
bool take_damage(int pips, Vector2 knockback_dir);

// Get the player's health
int get_health();

// Get the player's attack rect when attacking
Rectangle get_attack_rect();

// Check if the player is attacking
bool is_attacking();

// Animation control
void set_animation(PlayerState state);

// Damage handling - returns true if damage was dealt
bool take_damage(int amount, Vector2 knockback_dir);

}  // namespace player 