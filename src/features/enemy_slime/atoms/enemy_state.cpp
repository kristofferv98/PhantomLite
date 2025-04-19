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
    slime_spec.hp = 40;  // Increased HP for testing
    slime_spec.dmg = 5;
    slime_spec.speed = 100.0f;
    slime_spec.radius = 16.0f;
    slime_spec.width = 32.0f;
    slime_spec.height = 32.0f;
    slime_spec.detection_radius = 300.0f;
    slime_spec.attack_radius = 50.0f;
    slime_spec.attack_cooldown = 1.2f;
    slime_spec.animation_frames = 2;
    
    // Set behavior flags - Added STRAFE_TARGET for more interesting movement
    slime_spec.behavior_flags = 
        enemies::BehaviorFlags::WANDER_NOISE |
        enemies::BehaviorFlags::BASIC_CHASE |
        enemies::BehaviorFlags::MELEE_ATTACK |
        enemies::BehaviorFlags::SEPARATE_ALLIES |
        enemies::BehaviorFlags::AVOID_OBSTACLES |
        enemies::BehaviorFlags::STRAFE_TARGET;  // Added strafing behavior
        
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
        if (!enemy.active || enemy.hp <= 0) continue;
        
        // Store original position for collision resolution and movement detection
        Vector2 original_pos = enemy.position;
        
        // Reset weights for this frame
        enemy.reset_weights();
        
        bool is_steering = false;
        float dist_to_player = calculate_distance(enemy.position, player_pos);
        
        // Update attack state if already attacking
        if (enemy.attack_melee.attacking) {
            // If already in attack animation, continue it
            enemy.attack_melee.attack_timer += dt;
            
            // Attack animation finished
            if (enemy.attack_melee.attack_timer >= enemy.attack_melee.attack_duration) {
                enemy.attack_melee.attacking = false;
                // Attack cooldown was already started when attack began
            }
            
            // Skip other behaviors while attacking
            is_steering = true;
        } 
        // Not currently attacking, check if should start a new attack
        else if (static_cast<int>(enemy.spec->behavior_flags & enemies::BehaviorFlags::MELEE_ATTACK) != 0) {
            if (dist_to_player <= enemy.spec->attack_radius) {
                // In attack range - try to attack
                enemies::BehaviorResult result = attack_player(enemy, dt);
                    
                if (result != enemies::BehaviorResult::Failed) {
                    // Attack succeeded or is in progress
                    is_steering = true;
                }
            }
        }
        
        // If not attacking or in another exclusive state, determine movement behaviors
        if (!is_steering) {
            // Advanced chase with charge dash (if equipped)
            if (static_cast<int>(enemy.spec->behavior_flags & enemies::BehaviorFlags::CHARGE_DASH) != 0) {
                if (dist_to_player <= enemy.spec->detection_radius * 0.8f && 
                    dist_to_player > enemy.spec->attack_radius * 1.2f && 
                    (rand() % 100) < 3) { // 3% chance to initiate dash when in range
                    
                    // Try charge dash
                    enemies::BehaviorResult result = enemies::atoms::charge_dash(enemy, player_pos, dt);
                    if (result == enemies::BehaviorResult::Running) {
                        is_steering = true;
                    }
                }
            }
            
            // Strafe behavior (medium range)
            if (!is_steering && 
                static_cast<int>(enemy.spec->behavior_flags & enemies::BehaviorFlags::STRAFE_TARGET) != 0) {
                
                if (dist_to_player <= enemy.spec->detection_radius * 0.8f && 
                    dist_to_player > enemy.spec->attack_radius * 1.2f && 
                    dist_to_player < enemy.spec->detection_radius * 0.7f) {
                    
                    // Try strafing (orbit around player)
                    enemies::BehaviorResult result = strafe_player(enemy, dt);
                    if (result == enemies::BehaviorResult::Running) {
                        is_steering = true;
                    }
                }
            }
            
            // Basic chase (if in detection range and not currently strafing)
            if (!is_steering && 
                (static_cast<int>(enemy.spec->behavior_flags & enemies::BehaviorFlags::BASIC_CHASE) != 0 ||
                 static_cast<int>(enemy.spec->behavior_flags & enemies::BehaviorFlags::ADVANCED_CHASE) != 0)) {
                
                if (dist_to_player <= enemy.spec->detection_radius) {
                    // Player is within detection range - chase
                    enemies::BehaviorResult result = chase_player(enemy, dt);
                    if (result == enemies::BehaviorResult::Running) {
                        is_steering = true;
                    }
                }
            }
            
            // Wander behavior (always apply but with lower weight when another behavior is active)
            if (static_cast<int>(enemy.spec->behavior_flags & enemies::BehaviorFlags::WANDER_NOISE) != 0) {
                // Always have some wander, but reduce influence when another behavior is active
                float wander_strength = is_steering ? 0.3f : 1.0f;
                
                // Apply wandering with noise
                if (wander_strength > 0.0f) {
                    wander_noise(enemy, dt);
                }
            }
            
            // Obstacle avoidance (critical, should always be applied)
            if (static_cast<int>(enemy.spec->behavior_flags & enemies::BehaviorFlags::AVOID_OBSTACLES) != 0) {
                avoid_obstacles(enemy, dt);
            }
            
            // Separation from other enemies (critical, should always be applied)
            if (static_cast<int>(enemy.spec->behavior_flags & enemies::BehaviorFlags::SEPARATE_ALLIES) != 0) {
                separate_from_allies(enemy, dt);
            }
            
            // Apply the final context steering movement
            enemies::atoms::apply_context_steering(enemy, dt);
        }
        
        // Update attack cooldown timer if not already attacking
        if (!enemy.attack_melee.attacking && !enemy.attack_melee.can_attack) {
            enemy.attack_melee.timer += dt;
            if (enemy.attack_melee.timer >= enemy.attack_melee.cooldown) {
                enemy.attack_melee.timer = 0.0f;
                enemy.attack_melee.can_attack = true;
            }
        }
        
        // Reset enemy color if not attacking (usually red during attack)
        if (!enemy.attack_melee.attacking) {
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

const std::vector<enemies::EnemyRuntime>& get_enemies() {
    return enemies;
}

std::vector<enemies::EnemyRuntime>& get_enemies_mutable() {
    return enemies;
}

void cleanup_inactive_enemies() {
    // Remove dead enemies (move to end and erase)
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(), 
            [](const enemies::EnemyRuntime& enemy) { return !enemy.active || enemy.hp <= 0; }),
        enemies.end()
    );
}

int get_active_enemy_count() {
    return enemies.size();
}

bool apply_damage_at(const Rectangle& hit_rect, const enemies::Hit& hit) {
    bool hit_any = false;
    
    // Get player position for calculating knockback direction
    Vector2 player_pos = player::get_position();
    
    // Check each enemy for collision with the hit rectangle
    for (auto& enemy : enemies) {
        if (!enemy.active || enemy.hp <= 0) continue;
        
        // Check if hit rectangle intersects with enemy collision rectangle
        if (CheckCollisionRecs(hit_rect, enemy.collision_rect)) {
            // Calculate knockback direction from player to enemy (more realistic)
            Vector2 knockback_dir = {
                enemy.position.x - player_pos.x,
                enemy.position.y - player_pos.y
            };
            
            // Normalize direction
            float length = sqrtf(knockback_dir.x * knockback_dir.x + knockback_dir.y * knockback_dir.y);
            if (length > 0) {
                knockback_dir.x /= length;
                knockback_dir.y /= length;
            } else {
                // If somehow at same position, knock back to the right
                knockback_dir.x = 1.0f;
                knockback_dir.y = 0.0f;
            }
            
            // Scale knockback force based on hit strength
            float knockback_force = 200.0f;
            knockback_dir.x *= knockback_force;
            knockback_dir.y *= knockback_force;
            
            // Create a complete hit with knockback
            enemies::Hit full_hit = hit;
            full_hit.knockback = knockback_dir;
            
            // Apply hit to enemy
            enemy.on_hit(full_hit);
            hit_any = true;
            
            // Log the hit for debugging
            TraceLog(LOG_INFO, "Player hit an enemy!");
        }
    }
    
    return hit_any;
}

void clear_enemies() {
    enemies.clear();
}

const enemies::EnemyStats& get_slime_spec() {
    return slime_spec;
}

} // namespace atoms
} // namespace enemy 