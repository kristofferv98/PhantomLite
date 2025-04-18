#pragma once

#include <raylib.h>
#include <string>
#include <vector>
#include "../../enemies/types.hpp"

namespace enemy {

// Using the common enemy types from the enemies namespace
using EnemyType = enemies::EnemyType;
using Facing = enemies::Facing;

// Legacy Enemy data structure for backward compatibility
struct Enemy {
    Vector2 position;       // Current position
    Vector2 size;           // Collision box dimensions
    float health;           // Current health
    float maxHealth;        // Maximum health
    float damage;           // Damage dealt to player on contact
    float moveSpeed;        // Movement speed
    float hitTimer;         // Visual feedback timer when hit
    bool isActive;          // Whether the enemy is active in the world
    bool isVisible;         // Whether the enemy is visible
    bool isHit;             // Whether the enemy was recently hit
    Facing facing;          // Direction the enemy is facing
    EnemyType type;         // Type of enemy
};

} // namespace enemy 