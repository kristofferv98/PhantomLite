/// behavior_atoms.hpp — Enemy behavior building blocks

#pragma once

#include "../enemy_slime.hpp"

namespace enemy {
namespace atoms {

/// Randomly wander around the starting position
/// PERF: ~0.02-0.05ms per enemy
BehaviorResult wander_random(EnemyRuntime& enemy, float dt);

/// Chase the player when within detection radius
/// PERF: ~0.01-0.02ms per enemy
BehaviorResult chase_player(EnemyRuntime& enemy, float dt);

/// Attack the player when in range
/// PERF: ~0.01-0.02ms per enemy
BehaviorResult attack_player(EnemyRuntime& enemy, float dt);

} // namespace atoms
} // namespace enemy 