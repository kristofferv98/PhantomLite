/// behavior_atoms.hpp — Enemy behavior building blocks

#pragma once

#include "../enemy_slime.hpp"
#include "../../enemies/behavior_atoms.hpp"
#include <raylib.h>

namespace enemy {
namespace atoms {

// Forward declare player position accessor function
Vector2 get_player_position();

/// Randomly wander around the starting position
/// PERF: ~0.02-0.05ms per enemy
enemies::BehaviorResult wander_random(enemies::EnemyRuntime& enemy, float dt);

/// Chase the player when within detection radius
/// PERF: ~0.01-0.02ms per enemy
enemies::BehaviorResult chase_player(enemies::EnemyRuntime& enemy, float dt);

/// Attack the player when in range
/// PERF: ~0.01-0.02ms per enemy
enemies::BehaviorResult attack_player(enemies::EnemyRuntime& enemy, float dt);

// Toggle debug visualization
void toggle_debug_visualization();

// Set debug visualization state
void set_debug_visualization(bool enabled);

// Get debug visualization state
bool is_debug_visualization_enabled();

// Toggle steering debug visualization
void toggle_steering_debug();

// Get steering debug visualization state
bool is_steering_debug_enabled();

} // namespace atoms
} // namespace enemy 