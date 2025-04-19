/// enemy_state.cpp — Enemy state management atom implementation

#include "enemy_state.hpp"
#include "behavior_atoms.hpp"
#include "../enemy_slime.hpp"
#include "../../world/world.hpp"
#include "../../player/player.hpp"
#include "../../enemies/behavior_atoms.hpp"
#include <algorithm>
#include <cstdlib>
#include <random>

namespace enemy {
namespace atoms {

// Storage for all active enemy instances
static std::vector<enemies::EnemyRuntime> enemies;

// Define slime specification
// This will be initialized during the init_enemy_state function
static enemies::EnemyStats slime_spec;

void init_enemy_state() {
    // Clear any existing enemies
    enemies.clear();
    
    // Set random seed for reproducible behavior
    srand(42);
    
    // Initialize our slime specification
    slime_spec.id = enemies::EnemyID::FOR_SLIME;
    slime_spec.type = enemies::EnemyType::SLIME_SMALL;
    slime_spec.name = "Forest Slime";
    slime_spec.size = {32.0f, 32.0f};
    slime_spec.hp = 2;
    slime_spec.dmg = 1;
    slime_spec.speed = 60.0f;
    slime_spec.radius = 16.0f;
    slime_spec.width = 32.0f;
    slime_spec.height = 32.0f;
    slime_spec.detection_radius = 200.0f;
    slime_spec.attack_radius = 50.0f;
    slime_spec.attack_cooldown = 1.2f;
    slime_spec.animation_frames = 2;
    
    // Set behavior flags
    slime_spec.behavior_flags = 
        enemies::BehaviorFlags::WANDER_NOISE |
        enemies::BehaviorFlags::BASIC_CHASE |
        enemies::BehaviorFlags::MELEE_ATTACK |
        enemies::BehaviorFlags::AVOID_OBSTACLES;
        
    // Define drops
    slime_spec.drops = {
        enemies::DropChance{enemies::DropType::Heart, 30}, 
        enemies::DropChance{enemies::DropType::Coin, 70}
    };
}

// Helper function for calculating distance
float calculate_distance(const Vector2& a, const Vector2& b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return sqrtf(dx * dx + dy * dy);
}

// PERF: ~0.05-0.5ms depending on enemy count
void update_enemy_states(float dt) {
    Vector2 player_pos = player::get_position();
    
    for (auto& enemy : enemies) {
        if (!enemy.active) continue;
        
        // Store original position for collision resolution and movement detection
        Vector2 original_pos = enemy.position;
        
        // Reset weights for this frame
        enemy.reset_weights();
        
        // Check if enemy has the attack behavior and is in range
        if (static_cast<int>(enemy.spec->behavior_flags & enemies::BehaviorFlags::MELEE_ATTACK) != 0) {
            float dist_to_player = calculate_distance(enemy.position, player_pos);
            
            if (dist_to_player <= enemy.spec->attack_radius) {
                // In attack range - try to attack
                enemies::BehaviorResult result = 
                    enemies::atoms::attack_melee(enemy, player_pos, dt);
                    
                if (result != enemies::BehaviorResult::Failed) {
                    // Attack succeeded or is in progress, continue to next enemy
                    enemy.is_moving = (enemy.position.x != original_pos.x || 
                                     enemy.position.y != original_pos.y);
                    continue;
                }
            }
        }
        
        // If not attacking, determine movement behaviors
        
        // Chase behavior
        if (static_cast<int>(enemy.spec->behavior_flags & enemies::BehaviorFlags::BASIC_CHASE) != 0) {
            float dist_to_player = calculate_distance(enemy.position, player_pos);
            
            if (dist_to_player <= enemy.spec->detection_radius) {
                // Player is within detection range - chase
                enemies::atoms::apply_seek_weights(enemy, player_pos, 1.0f);
            }
        } else if (static_cast<int>(enemy.spec->behavior_flags & enemies::BehaviorFlags::ADVANCED_CHASE) != 0) {
            float dist_to_player = calculate_distance(enemy.position, player_pos);
            
            if (dist_to_player <= enemy.spec->detection_radius) {
                // Player is within detection range
                if (dist_to_player > enemy.spec->attack_radius * 1.5f) {
                    // Far enough - seek player
                    enemies::atoms::apply_seek_weights(enemy, player_pos, 1.0f);
                } else {
                    // Close enough - strafe around player
                    enemies::atoms::apply_strafe_weights(
                        enemy, player_pos, 
                        enemy.strafe_target.direction, 
                        enemy.strafe_target.orbit_gain
                    );
                }
            }
        }
        
        // Wander behavior (only if not chasing)
        if (static_cast<int>(enemy.spec->behavior_flags & enemies::BehaviorFlags::WANDER_NOISE) != 0) {
            float dist_to_player = calculate_distance(enemy.position, player_pos);
            
            if (dist_to_player > enemy.spec->detection_radius) {
                // Not chasing player, apply wandering
                enemies::atoms::wander_noise(enemy, dt);
            }
        }
        
        // Obstacle avoidance (applied on top of other behaviors)
        if (static_cast<int>(enemy.spec->behavior_flags & enemies::BehaviorFlags::AVOID_OBSTACLES) != 0) {
            enemies::atoms::apply_obstacle_avoidance_weights(
                enemy, 
                enemy.avoid_obstacle.lookahead_px, 
                enemy.avoid_obstacle.avoidance_gain
            );
        }
        
        // Separation from other enemies (optional)
        if (static_cast<int>(enemy.spec->behavior_flags & enemies::BehaviorFlags::SEPARATE_ALLIES) != 0) {
            enemies::atoms::apply_separation_weights(
                enemy, 
                enemies, 
                enemy.separate_allies.desired_spacing, 
                enemy.separate_allies.separation_gain
            );
        }
        
        // Apply the final movement
        enemies::atoms::apply_context_steering(enemy, dt);
        
        // Reset enemy color if not attacking (usually red during attack)
        if (!enemy.attack.attacking) {
            enemy.color = WHITE; // Default sprite color
        } else {
            // Use a more subtle red tint that works better with sprites
            enemy.color = {255, 150, 150, 255}; // Light red tint
        }
        
        // Check world boundaries - using proper world bounds
        float min_x, min_y, max_x, max_y;
        world::get_world_bounds(&min_x, &min_y, &max_x, &max_y);
        
        float half_width = enemy.spec->size.x / 2;
        float half_height = enemy.spec->size.y / 2;
        
        // Constrain position to world boundaries
        if (enemy.position.x - half_width < min_x) {
            enemy.position.x = min_x + half_width;
        } else if (enemy.position.x + half_width > max_x) {
            enemy.position.x = max_x - half_width;
        }
        
        if (enemy.position.y - half_height < min_y) {
            enemy.position.y = min_y + half_height;
        } else if (enemy.position.y + half_height > max_y) {
            enemy.position.y = max_y - half_height;
        }
        
        // Update collision rectangle after movement
        enemy.collision_rect.x = enemy.position.x - enemy.spec->size.x/2;
        enemy.collision_rect.y = enemy.position.y - enemy.spec->size.y/2;
        enemy.collision_rect.width = enemy.spec->size.x;
        enemy.collision_rect.height = enemy.spec->size.y;
        
        // Detect if the enemy is moving
        enemy.is_moving = (enemy.position.x != original_pos.x || enemy.position.y != original_pos.y);
        
        // Update animation timer and frame
        const float ANIMATION_FRAME_TIME = 0.25f; // Seconds per frame
        enemy.anim_timer += dt;
        if (enemy.anim_timer >= ANIMATION_FRAME_TIME) {
            enemy.anim_timer = 0;
            enemy.anim_frame = (enemy.anim_frame + 1) % enemy.spec->animation_frames;
        }
    }
}

void add_enemy(const enemies::EnemyRuntime& enemy) {
    enemies.push_back(enemy);
}

void cleanup_inactive_enemies() {
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(), [](const enemies::EnemyRuntime& enemy) {
            return !enemy.active || enemy.hp <= 0;
        }),
        enemies.end()
    );
}

int get_active_enemy_count() {
    return enemies.size();
}

bool apply_damage_at(const Rectangle& hit_rect, const enemies::Hit& hit) {
    bool any_hit = false;
    
    for (auto& enemy : enemies) {
        if (CheckCollisionRecs(enemy.collision_rect, hit_rect)) {
            // Use the on_hit method to apply damage and knockback
            enemy.on_hit(hit);
            
            any_hit = true;
        }
    }
    
    return any_hit;
}

const std::vector<enemies::EnemyRuntime>& get_enemies() {
    return enemies;
}

std::vector<enemies::EnemyRuntime>& get_enemies_mutable() {
    return enemies;
}

void clear_enemies() {
    enemies.clear();
}

const enemies::EnemyStats& get_slime_spec() {
    return slime_spec;
}

} // namespace atoms
} // namespace enemy 



