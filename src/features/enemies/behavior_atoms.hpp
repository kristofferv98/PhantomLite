/// behavior_atoms.hpp — Common behavior building blocks for all enemy types
#pragma once

#include "types.hpp"
#include "../enemy_slime/atoms/enemy_combat.hpp"
#include "../world/world.hpp"
#include <raylib.h>
#include <vector>

namespace enemies {
namespace atoms {

/// Apply wander behavior using Perlin noise for smooth paths
/// PERF: ~0.02-0.05ms per enemy
BehaviorResult wander_noise(EnemyRuntime& enemy, float dt);

/// Seek toward a target position
/// PERF: ~0.01-0.02ms per enemy
BehaviorResult seek_target(EnemyRuntime& enemy, Vector2 target, float dt);

/// Strafe/orbit around a target position
/// PERF: ~0.01-0.02ms per enemy
BehaviorResult strafe_target(EnemyRuntime& enemy, Vector2 target, float dt);

/// Maintain distance from other enemies
/// PERF: ~0.05-0.15ms per enemy (scales with nearby entity count)
BehaviorResult separate_allies(EnemyRuntime& enemy, const std::vector<EnemyRuntime>& enemies, float dt);

/// Avoid obstacles using raycasts
/// PERF: ~0.05-0.1ms per enemy (depends on obstacle complexity)
BehaviorResult avoid_obstacles(EnemyRuntime& enemy, float dt);

/// Apply a charge and dash attack
/// PERF: ~0.01-0.02ms per enemy
BehaviorResult charge_dash(EnemyRuntime& enemy, Vector2 target, float dt);

/// Apply a ranged attack
/// PERF: ~0.01-0.02ms per enemy (projectile creation separate)
BehaviorResult ranged_shoot(EnemyRuntime& enemy, Vector2 target, float dt);

/// Apply a melee attack
/// PERF: ~0.01-0.02ms per enemy
BehaviorResult attack_melee(EnemyRuntime& enemy, Vector2 target, float dt);

/// Process the steering weights and apply movement
/// PERF: ~0.02-0.03ms per enemy
BehaviorResult apply_context_steering(EnemyRuntime& enemy, float dt);

/// Debug visualization for steering weights
void draw_steering_weights(const EnemyRuntime& enemy, bool screen_space = true);

// Helper functions for steering weights

/// Apply weights for seeking a target
void apply_seek_weights(EnemyRuntime& enemy, Vector2 target, float gain = 1.0f);

/// Apply weights for strafing around a target
void apply_strafe_weights(EnemyRuntime& enemy, Vector2 target, int direction, float gain = 1.0f);

/// Apply weights for separation from other entities
void apply_separation_weights(EnemyRuntime& enemy, const std::vector<EnemyRuntime>& enemies, float desired_dist, float gain = 1.0f);

/// Apply weights for obstacle avoidance using raycasts
void apply_obstacle_avoidance_weights(EnemyRuntime& enemy, float lookahead_dist, float gain = 1.0f);

/// Apply weights for a specific direction vector
void apply_direction_weights(EnemyRuntime& enemy, Vector2 direction, float gain = 1.0f);

// Helper for context steering
void apply_steering_movement(EnemyRuntime& enemy, float dt);

} // namespace atoms
} // namespace enemies 