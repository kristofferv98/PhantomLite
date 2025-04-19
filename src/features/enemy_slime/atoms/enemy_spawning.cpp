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

// Cached enemy specifications
static std::vector<enemies::EnemyStats> slime_specs;

// Internal constants for spawning
static const float MIN_SPAWN_DISTANCE = 300.0f;
static const float MAX_SPAWN_DISTANCE = 800.0f;
static const int SPAWN_ATTEMPTS = 10;

void init_spawning() {
    // Seed random number generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    // Initialize slime specifications if not already done
    if (slime_specs.empty()) {
        // Small slime
        {
            enemies::EnemyStats small_slime;
            small_slime.id = enemies::EnemyID::FOR_SLIME;
            small_slime.type = enemies::EnemyType::SLIME_SMALL;
            small_slime.name = "Small Slime";
            small_slime.size = {32.0f, 32.0f};
            small_slime.hp = 20;
            small_slime.dmg = 5;
            small_slime.speed = 100.0f;
            small_slime.detection_radius = 300.0f;
            small_slime.attack_radius = 50.0f;
            small_slime.attack_cooldown = 2.0f;
            
            // Assign behaviors
            small_slime.behavior_flags = 
                enemies::BehaviorFlags::WANDER_NOISE |
                enemies::BehaviorFlags::BASIC_CHASE |
                enemies::BehaviorFlags::MELEE_ATTACK;
                
            slime_specs.push_back(small_slime);
        }
        
        // Medium slime
        {
            enemies::EnemyStats medium_slime;
            medium_slime.id = enemies::EnemyID::FOR_SLIME;
            medium_slime.type = enemies::EnemyType::SLIME_MEDIUM;
            medium_slime.name = "Medium Slime";
            medium_slime.size = {48.0f, 48.0f};
            medium_slime.hp = 40;
            medium_slime.dmg = 10;
            medium_slime.speed = 80.0f;
            medium_slime.detection_radius = 350.0f;
            medium_slime.attack_radius = 60.0f;
            medium_slime.attack_cooldown = 1.8f;
            
            // Assign behaviors - medium slimes will strafe more
            medium_slime.behavior_flags = 
                enemies::BehaviorFlags::WANDER_NOISE |
                enemies::BehaviorFlags::ADVANCED_CHASE |
                enemies::BehaviorFlags::MELEE_ATTACK |
                enemies::BehaviorFlags::STRAFE_TARGET;
                
            slime_specs.push_back(medium_slime);
        }
        
        // Large slime
        {
            enemies::EnemyStats large_slime;
            large_slime.id = enemies::EnemyID::FOR_SLIME;
            large_slime.type = enemies::EnemyType::SLIME_LARGE;
            large_slime.name = "Large Slime";
            large_slime.size = {64.0f, 64.0f};
            large_slime.hp = 80;
            large_slime.dmg = 15;
            large_slime.speed = 60.0f;
            large_slime.detection_radius = 400.0f;
            large_slime.attack_radius = 70.0f;
            large_slime.attack_cooldown = 2.5f;
            
            // Assign behaviors - large slimes will charge
            large_slime.behavior_flags = 
                enemies::BehaviorFlags::WANDER_NOISE |
                enemies::BehaviorFlags::ADVANCED_CHASE |
                enemies::BehaviorFlags::CHARGE_DASH |
                enemies::BehaviorFlags::MELEE_ATTACK;
                
            slime_specs.push_back(large_slime);
        }
    }
}

enemies::EnemyRuntime spawn_enemy(Vector2 position, enemies::EnemyType type) {
    // Find the spec for the requested type
    for (const auto& spec : slime_specs) {
        if (spec.type == type) {
            // Create a new enemy instance with the spec
            enemies::EnemyRuntime new_enemy(spec, position);
            
            // Initialize behavior-specific parameters
            if (static_cast<int>(spec.behavior_flags & enemies::BehaviorFlags::WANDER_NOISE) != 0) {
                // Increase radius for more room to wander
                new_enemy.wander_noise.radius = 250.0f;
                // Reduce sway speed for smoother, more natural movement
                new_enemy.wander_noise.sway_speed = 0.4f;
                // Ensure spawn point is properly set
                new_enemy.wander_noise.spawn_point = position;
                // Set initial noise offsets to different random values to avoid synchronized movement
                new_enemy.wander_noise.noise_offset_x = static_cast<float>(GetRandomValue(0, 1000)) / 100.0f;
                new_enemy.wander_noise.noise_offset_y = static_cast<float>(GetRandomValue(0, 1000)) / 100.0f;
            }
            
            if (static_cast<int>(spec.behavior_flags & enemies::BehaviorFlags::STRAFE_TARGET) != 0) {
                // Slightly smaller orbit radius for tighter circling
                new_enemy.strafe_target.orbit_radius = 80.0f;
                // Balanced gain to not overpower other behaviors
                new_enemy.strafe_target.orbit_gain = 0.65f;
                // Random orbit direction (1=clockwise, -1=counterclockwise)
                new_enemy.strafe_target.direction = GetRandomValue(0, 1) ? 1 : -1;
            }
            
            if (static_cast<int>(spec.behavior_flags & enemies::BehaviorFlags::SEPARATE_ALLIES) != 0) {
                // Increased spacing to reduce clumping
                new_enemy.separate_allies.desired_spacing = spec.radius * 3.5f;
                // Increased separation gain to make this behavior strong
                new_enemy.separate_allies.separation_gain = 1.4f;
            }
            
            if (static_cast<int>(spec.behavior_flags & enemies::BehaviorFlags::CHARGE_DASH) != 0) {
                new_enemy.charge_dash.charge_duration = 1.0f;
                new_enemy.charge_dash.dash_speed = 3.0f;
                new_enemy.charge_dash.dash_duration = 0.5f;
            }
            
            if (static_cast<int>(spec.behavior_flags & enemies::BehaviorFlags::AVOID_OBSTACLES) != 0) {
                // Increased lookahead distance for earlier detection
                new_enemy.avoid_obstacle.lookahead_px = 120.0f;
                // Strong avoidance gain to prioritize obstacle avoidance
                new_enemy.avoid_obstacle.avoidance_gain = 1.7f;
            }
            
            TraceLog(LOG_INFO, "Spawned %s at position: (%.2f, %.2f)", spec.name.c_str(), position.x, position.y);
            return new_enemy;
        }
    }
    
    // If type not found, default to small slime
    TraceLog(LOG_WARNING, "Enemy type not found, defaulting to small slime");
    return spawn_enemy(position, enemies::EnemyType::SLIME_SMALL);
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
            
            // Choose enemy type based on difficulty
            enemies::EnemyType type;
            int roll = GetRandomValue(1, 100);
            
            if (roll <= 10 + static_cast<int>(difficulty / 5.0f)) {
                type = enemies::EnemyType::SLIME_LARGE;
            } else if (roll <= 40 + static_cast<int>(difficulty / 2.0f)) {
                type = enemies::EnemyType::SLIME_MEDIUM;
            } else {
                type = enemies::EnemyType::SLIME_SMALL;
            }
            
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