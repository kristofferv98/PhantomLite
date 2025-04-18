#pragma once

#include <raylib.h>
#include <vector>
#include "../../enemies/types.hpp"

namespace enemy::atoms {

/**
 * Initialize the enemy spawning system.
 * This must be called before any enemy can be spawned.
 */
void init_spawning();

/**
 * Spawn a new enemy at the specified position.
 * 
 * @param position The position to spawn the enemy at
 * @param type The type of enemy to spawn
 * @return The newly created enemy runtime instance
 */
enemies::EnemyRuntime spawn_enemy(Vector2 position, enemies::EnemyType type);

/**
 * Spawn enemies around the player based on difficulty.
 * 
 * @param player_position The position of the player
 * @param difficulty The current difficulty level (higher means more enemies)
 * @param enemies Reference to the vector of enemies to add to
 * @param maxEnemies Maximum number of enemies allowed
 */
void spawn_enemies_around_player(const Vector2& player_position, 
                               float difficulty, 
                               std::vector<enemies::EnemyRuntime>& enemies,
                               int maxEnemies);

/**
 * Clean up resources used by the enemy spawning system.
 */
void cleanup_spawning();

} // namespace enemy::atoms 