/// behavior_atoms.hpp — Enemy behavior building blocks

#pragma once

#include "raylib.h"
#include <cstdint>
#include <functional>

namespace enemy {

// Forward declarations
struct EnemyInstance;

/// Behavior result indicating what to do next
enum class BehaviorResult {
    Running,    ///< Behavior still executing, keep calling
    Completed,  ///< Behavior finished successfully
    Failed      ///< Behavior couldn't be completed
};

/// Behavior atom function signature
using BehaviorAtomFn = std::function<BehaviorResult(EnemyInstance&, float)>;

/// Wandering behavior parameters
struct WanderRandom {
    float radius;        ///< Maximum distance to wander from current position
    float idle_time;     ///< Time to wait between movements
    float current_timer; ///< Current timer (counts down)
    Vector2 target;      ///< Current movement target
    bool has_target;     ///< Whether we have a valid target
    
    WanderRandom(float radius = 100.0f, float idle_time = 1.0f)
        : radius(radius), idle_time(idle_time), current_timer(0.0f), 
          target({0, 0}), has_target(false) {}
};

/// Player chasing behavior parameters
struct ChasePlayer {
    float detection_radius; ///< Distance at which player is detected
    bool chasing;           ///< Whether currently chasing player
    
    ChasePlayer(float detection_radius = 200.0f)
        : detection_radius(detection_radius), chasing(false) {}
};

/// Player attack behavior parameters
struct AttackPlayer {
    float attack_radius;    ///< Distance at which enemy can attack player
    float cooldown;         ///< Time between attacks in seconds
    float timer;            ///< Current cooldown timer
    bool can_attack;        ///< Whether enemy can attack now
    bool attacking;         ///< Whether currently in attack animation
    
    AttackPlayer(float attack_radius = 40.0f, float cooldown = 1.0f)
        : attack_radius(attack_radius), cooldown(cooldown), timer(0.0f), 
          can_attack(true), attacking(false) {}
};

/// Behavior atoms - these are the building blocks of enemy behavior
namespace atoms {

/// Randomly wander around the starting position
BehaviorResult wander_random(EnemyInstance& enemy, float dt);

/// Chase the player when within detection radius
BehaviorResult chase_player(EnemyInstance& enemy, float dt);

/// Attack the player when in range
BehaviorResult attack_player(EnemyInstance& enemy, float dt);

} // namespace atoms

} // namespace enemy 