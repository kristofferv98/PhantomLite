/// enemy_slime.hpp — Forest Slime enemy (public API)

#pragma once

#include "raylib.h"
#include "features/enemies/types.hpp"
#include <vector>

namespace enemy {

// Using the common enemy types
using EnemyID = enemies::EnemyID;
using DropType = enemies::DropType;
using DropChance = enemies::DropChance;
using BehaviorAtom = enemies::BehaviorAtom;
using EnemyStats = enemies::EnemyStats;
using EnemyRuntime = enemies::EnemyRuntime;
using Hit = enemies::Hit;
using BehaviorResult = enemies::BehaviorResult;

/// Initialize the enemy system
void init_enemies();

/// Update all active enemies
void update_enemies(float dt);

/// Render all active enemies
void render_enemies();

/// Spawn a slime at the given position
void spawn_slime(Vector2 position);

/// Spawn multiple slimes in random positions for demo purposes
void spawn_demo_slimes(int count);

/// Toggle visibility of debug information
void toggle_debug_info();

/// Toggle steering debug visualization (ray weights)
void toggle_steering_debug();

/// Process a hit on an enemy at the given position, returns true if hit successful
bool hit_enemy_at(const Rectangle& hit_rect, const Hit& hit);

/// Cleanup enemy resources
void cleanup_enemies();

/// Get the slime enemy specification
const EnemyStats& get_slime_spec();

/// Initialize the enemy system
void init();

/// Update all enemies
void update(float delta_time);

/// Render all enemies
void render();

/// Cleanup resources
void cleanup();

/// Check if an enemy is colliding with the player
bool check_player_collision(Vector2 position, float radius, int* enemy_id = nullptr);

/// Enemy takes damage
void take_damage(int enemy_id, int damage);

/// Toggle debug visualization
void toggle_debug();

/// Set debug visualization state
void set_debug(bool enabled);

/// Get debug visualization state
bool is_debug_enabled();

/// Get steering debug visualization state
bool is_steering_debug_enabled();

/// Get number of active enemies
int get_enemy_count();

} // namespace enemy 