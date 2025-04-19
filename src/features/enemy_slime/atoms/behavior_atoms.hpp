/// behavior_atoms.hpp — Enemy behavior building blocks

#pragma once

#include "../enemy_slime.hpp"
#include "../../enemies/behavior_atoms.hpp"
#include <raylib.h>

namespace enemy {
    // Forward declaration
    struct EnemyInstance;

namespace atoms {

// Forward declare player position accessor function
Vector2 get_player_position();

/// Randomly wander around the starting position
/// PERF: ~0.02-0.05ms per enemy
enemies::BehaviorResult wander_random(enemies::EnemyRuntime& enemy, float dt);

/// Chase the player when within detection radius
/// PERF: ~0.01-0.02ms per enemy
enemies::BehaviorResult chase_player(enemies::EnemyRuntime& enemy, float dt);

/// Chase the player with enhanced obstacle avoidance
/// Uses the world::raycast system for better terrain navigation
/// PERF: ~0.05-0.1ms per enemy (more expensive but more intelligent)
enemies::BehaviorResult chase_player_smart(enemies::EnemyRuntime& enemy, float dt);

/// Enhanced obstacle avoidance implementation using world::raycast
/// Provides better navigation around complex terrain and smoother movement
/// PERF: ~0.05-0.1ms per enemy
enemies::BehaviorResult enhanced_obstacle_avoidance(enemies::EnemyRuntime& enemy, float dt);

/// Attack the player when in range
/// PERF: ~0.01-0.02ms per enemy
enemies::BehaviorResult attack_player(enemies::EnemyRuntime& enemy, float dt);

/// Get the time elapsed since startup (used for animations, etc.)
float get_elapsed_time();

/// Attack the player when in range using the core entity adapter
/// Returns true if attack was attempted successfully
bool attack_player_with_adapter(EnemyRuntime& enemy, Vector2 player_pos, float dt);

/// Toggle debug visualization for enemy behaviors
void toggle_debug_visualization();

/// Set debug visualization flag
void set_debug_visualization(bool enabled);

/// Get debug visualization status
bool is_debug_visualization_enabled();

/// Toggle steering debug visualization
void toggle_steering_debug();

/// Get steering debug visualization state
bool is_steering_debug_enabled();

/// Toggle obstacle avoidance visualization
void toggle_obstacle_avoidance_visualization();

/// Global flag for obstacle avoidance visualization
extern bool g_show_obstacle_avoidance;

} // namespace atoms
} // namespace enemy 