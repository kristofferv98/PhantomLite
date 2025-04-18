/// enemy_spawner.cpp — Enemy spawning atom implementation

#include "enemy_spawner.hpp"
#include "enemy_state.hpp"
#include "enemy_spawning.hpp"
#include "../../player/player.hpp"
#include "../../world/world.hpp"
#include <random>
#include <cmath>

namespace enemy {
namespace atoms {

void init_spawner() {
    // Nothing specific needed for initialization
}

void spawn_slime_at(Vector2 position) {
    // Use the new enemy_spawning implementation to create an enemy
    enemies::EnemyRuntime new_slime = spawn_enemy(position, enemies::EnemyType::SLIME_SMALL);
    
    // Add to the enemy state system
    add_enemy(new_slime);
}

void spawn_demo_slimes(int count) {
    // Log spawn count
    TraceLog(LOG_INFO, "spawn_demo_slimes: Spawning %d slimes", count);
    
    // Get player position
    Vector2 player_pos = player::get_position();
    
    // Get all existing enemies
    std::vector<enemies::EnemyRuntime> enemies = get_enemies_mutable();
    
    // Use the new spawning implementation
    spawn_enemies_around_player(player_pos, 10.0f, enemies, count);
    
    // Clear existing enemies and add the new ones
    clear_enemies();
    for (const auto& enemy : enemies) {
        add_enemy(enemy);
    }
}

} // namespace atoms
} // namespace enemy 