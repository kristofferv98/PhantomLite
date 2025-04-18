/// enemy_state.hpp — Enemy state management atom

#pragma once

#include "../enemy_slime.hpp"
#include <vector>

namespace enemy {
namespace atoms {

/// Initialize the enemy state system
void init_enemy_state();

/// Update the state of all active enemies
/// PERF: ~0.05-0.5ms depending on enemy count
void update_enemy_states(float dt);

/// Add a new enemy instance to the state system
void add_enemy(const EnemyRuntime& enemy);

/// Get all active enemy instances
const std::vector<EnemyRuntime>& get_enemies();

/// Get a mutable reference to all enemy instances
/// This should be used carefully, only when direct modifications are necessary
std::vector<EnemyRuntime>& get_enemies_mutable();

/// Remove inactive or dead enemies
void cleanup_inactive_enemies();

/// Get the number of active enemies
int get_active_enemy_count();

/// Apply damage to an enemy at the given position
/// Returns true if an enemy was hit
bool apply_damage_at(const Rectangle& hit_rect, const Hit& hit);

/// Clear all enemy instances
void clear_enemies();

/// Get the slime specification (static data)
const EnemyStats& get_slime_spec();

} // namespace atoms
} // namespace enemy 