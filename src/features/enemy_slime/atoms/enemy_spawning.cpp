#include "enemy_spawning.hpp"
#include "../enemy_slime.hpp"
#include "../model/enemy_data.hpp"
#include "../../enemies/types.hpp"
#include "../../enemies/behavior_atoms.hpp"
#include "../../player/player.hpp"
#include "../../world/world.hpp"
#include <raymath.h>
#include <cstdlib>
#include <ctime>
#include <vector>

namespace enemy::atoms {

// Define our slime specifications - accessible by enemy_state.cpp via extern
std::vector<enemies::EnemyStats> slime_specs;

// Internal constants for spawning
static const float MIN_SPAWN_DISTANCE = 300.0f;
static const float MAX_SPAWN_DISTANCE = 800.0f;
static const int SPAWN_ATTEMPTS = 10;

void init_spawning() {
    // Clear any existing specs
    slime_specs.clear();

    // Small slime - the only slime type we're using
    enemies::EnemyStats small_slime;
    small_slime.id = enemies::EnemyID::FOR_SLIME;
    small_slime.type = enemies::EnemyType::SLIME_SMALL;
    small_slime.name = "Forest Slime";
    small_slime.size = {32.0f, 32.0f};
    small_slime.hp = 2;
    small_slime.dmg = 1; // This is the intended damage value
    small_slime.speed = 60.0f;
    small_slime.radius = 16.0f;
    small_slime.width = 32.0f;
    small_slime.height = 32.0f;
    small_slime.detection_radius = 200.0f;
    small_slime.attack_radius = 50.0f;
    small_slime.attack_cooldown = 1.2f;
    small_slime.animation_frames = 2;
    
    // Set behavior flags
    small_slime.behavior_flags = 
        enemies::BehaviorFlags::WANDER_NOISE |
        enemies::BehaviorFlags::BASIC_CHASE |
        enemies::BehaviorFlags::MELEE_ATTACK |
        enemies::BehaviorFlags::AVOID_OBSTACLES;
        
    // Define drops
    small_slime.drops = {
        enemies::DropChance{enemies::DropType::Heart, 30}, 
        enemies::DropChance{enemies::DropType::Coin, 70}
    };
    
    // Add to our specifications list
    slime_specs.push_back(small_slime);

    // We no longer define medium and large slimes
}

enemies::EnemyRuntime spawn_enemy(Vector2 position, enemies::EnemyType type) {
    // We ignore the requested type and always use the small slime (index 0)
    // This ensures we only use the small slime with dmg=1
    
    if (slime_specs.empty()) {
        init_spawning();
    }
    
    // Make sure there's a valid spawn position
    if (position.x < 0 || position.y < 0) {
        // Use a default position if invalid
        position = {100.0f, 100.0f};
    }
    
    // Create a new enemy instance using the constructor
    // Always use the small slime spec (index 0) regardless of requested type
    enemies::EnemyRuntime new_slime(slime_specs[0], position);
    
    // Log spawn information
    TraceLog(LOG_INFO, "Spawned %s at position: (%.2f, %.2f)", 
             slime_specs[0].name.c_str(), position.x, position.y);
    
    return new_slime;
}

void spawn_enemies_around_player(const Vector2& player_position, 
                               float difficulty, 
                               std::vector<enemies::EnemyRuntime>& enemies,
                               int maxEnemies) {
    // Don't spawn if we already have max enemies
    if (static_cast<int>(enemies.size()) >= maxEnemies) {
        return;
    }
    
    // Calculate how many enemies to try spawning based on difficulty
    int enemiesToSpawn = static_cast<int>(difficulty / 10.0f) + 1;
    enemiesToSpawn = std::min(enemiesToSpawn, maxEnemies - static_cast<int>(enemies.size()));
    
    for (int i = 0; i < enemiesToSpawn; i++) {
        // Try multiple times to find a valid spawn position
        for (int attempt = 0; attempt < SPAWN_ATTEMPTS; attempt++) {
            // Generate random angle and distance
            float angle = static_cast<float>(GetRandomValue(0, 360)) * DEG2RAD;
            float distance = static_cast<float>(GetRandomValue(
                static_cast<int>(MIN_SPAWN_DISTANCE), 
                static_cast<int>(MAX_SPAWN_DISTANCE)));
            
            // Calculate position
            Vector2 spawn_pos = {
                player_position.x + cosf(angle) * distance,
                player_position.y + sinf(angle) * distance
            };
            
            // Always use small slimes
            enemies::EnemyType type = enemies::EnemyType::SLIME_SMALL;
            
            // Verify spawn position is walkable
            if (world::is_walkable(spawn_pos.x, spawn_pos.y)) {
                // Spawn the enemy and add to list
                enemies.push_back(spawn_enemy(spawn_pos, type));
                break;
            }
        }
    }
}

void cleanup_spawning() {
    // Clear cached specifications
    slime_specs.clear();
}

} // namespace enemy::atoms 
