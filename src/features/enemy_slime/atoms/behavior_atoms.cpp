/// behavior_atoms.cpp — Implementation of enemy behavior building blocks

#include "behavior_atoms.hpp"
#include "../enemy_slime.hpp"
#include "../../player/player.hpp"
#include "../../enemies/behavior_atoms.hpp"
#include "../../world/world.hpp"
#include "../../core/public/entity.hpp"
#include <cmath>
#include <raymath.h>

namespace enemy {
namespace atoms {

// Debug flags
static bool show_debug = false;
static bool show_steering_debug = false;
bool g_show_obstacle_avoidance = true; // Implementation of the extern variable

Vector2 get_player_position() {
    return player::get_position();
}

// New improved obstacle avoidance for slimes using the world::raycast function
enemies::BehaviorResult enhanced_obstacle_avoidance(enemies::EnemyRuntime& enemy, float dt) {
    // Reset weights
    enemy.reset_weights();
    
    // Get player position - this is our target to seek
    Vector2 player_pos = get_player_position();
    
    // First apply seeking behavior toward player (positive weights in player direction)
    Vector2 to_player = { 
        player_pos.x - enemy.position.x,
        player_pos.y - enemy.position.y
    };
    float dist_to_player = Vector2Distance(enemy.position, player_pos);
    
    // Normalize direction
    Vector2 dir_to_player;
    if (dist_to_player > 0) {
        dir_to_player.x = to_player.x / dist_to_player;
        dir_to_player.y = to_player.y / dist_to_player;
    } else {
        dir_to_player = {0, 0};
    }
    
    // Apply seeking weights (stronger in direction of player)
    for (int i = 0; i < enemy.NUM_RAYS; i++) {
        // Get ray direction
        Vector2 ray_dir = enemy.get_ray_dir(i);
        
        // Calculate dot product (alignment between ray and player direction)
        float dot = ray_dir.x * dir_to_player.x + ray_dir.y * dir_to_player.y;
        
        // Apply weight (higher when ray points toward player)
        enemy.weights[i] += powf(fmaxf(0.0f, dot), 2.0f) * 1.5f;
    }
    
    // Now apply obstacle avoidance using world::raycast
    const float FAR_LOOKAHEAD = 150.0f;   // Look further ahead
    const float NEAR_LOOKAHEAD = 50.0f;   // Look nearby
    
    // Cast rays in all directions for obstacle detection
    for (int i = 0; i < enemy.NUM_RAYS; i++) {
        // Get ray direction
        Vector2 ray_dir = enemy.get_ray_dir(i);
        
        // Use world::raycast to get distance to obstacles
        float distance = world::raycast(enemy.position, ray_dir, FAR_LOOKAHEAD);
        
        // Apply strong avoidance for nearby obstacles
        if (distance < NEAR_LOOKAHEAD) {
            float closeness = 1.0f - (distance / NEAR_LOOKAHEAD);
            float avoidance_weight = -3.0f * closeness * closeness; // Stronger avoidance when closer
            enemy.weights[i] += avoidance_weight;
        }
        // Apply moderate avoidance for farther obstacles
        else if (distance < FAR_LOOKAHEAD) {
            float closeness = 1.0f - (distance / FAR_LOOKAHEAD);
            float avoidance_weight = -1.0f * closeness; // Linear falloff for farther obstacles
            enemy.weights[i] += avoidance_weight;
        }
        
        // Debug visualization if enabled
        if (g_show_obstacle_avoidance && show_debug) {
            Color ray_color = (distance < FAR_LOOKAHEAD) ? 
                MAROON : // Obstacle detected
                GREEN;   // Clear path
                
            Vector2 ray_end;
            if (distance < FAR_LOOKAHEAD) {
                ray_end.x = enemy.position.x + ray_dir.x * distance;
                ray_end.y = enemy.position.y + ray_dir.y * distance;
            } else {
                ray_end.x = enemy.position.x + ray_dir.x * FAR_LOOKAHEAD;
                ray_end.y = enemy.position.y + ray_dir.y * FAR_LOOKAHEAD;
            }
            
            // Convert positions to screen space for drawing
            Vector2 screen_pos = world::world_to_screen(enemy.position);
            Vector2 screen_end = world::world_to_screen(ray_end);
            
            DrawLineEx(screen_pos, screen_end, 1.0f, ray_color);
        }
    }
    
    // If close to player, apply strafing motion (orbit around player)
    if (dist_to_player < 100.0f) {
        // Calculate perpendicular direction (for orbiting)
        Vector2 strafe_dir = { -dir_to_player.y, dir_to_player.x }; // 90 degrees clockwise
        
        // Apply sideways movement weights
        for (int i = 0; i < enemy.NUM_RAYS; i++) {
            Vector2 ray_dir = enemy.get_ray_dir(i);
            
            // Calculate alignment with strafe direction
            float dot = ray_dir.x * strafe_dir.x + ray_dir.y * strafe_dir.y;
            
            // Apply strafe weight (higher in strafe direction)
            enemy.weights[i] += fmaxf(0.0f, dot) * 1.2f;
        }
    }
    
    // Apply movement based on weights
    enemies::atoms::apply_context_steering(enemy, dt);
    
    // Debug visualization of steering weights
    if (show_steering_debug) {
        enemies::atoms::draw_steering_weights(enemy, true); // true = screen space
    }
    
    return enemies::BehaviorResult::Running;
}

enemies::BehaviorResult wander_random(enemies::EnemyRuntime& enemy, float dt) {
    // Forward to the updated wander_noise implementation
    return enemies::atoms::wander_noise(enemy, dt);
}

enemies::BehaviorResult chase_player(enemies::EnemyRuntime& enemy, float dt) {
    // Get player position
    Vector2 player_pos = get_player_position();
    
    // Calculate distance to player
    float dist = Vector2Distance(enemy.position, player_pos);
    
    // If player is within detection radius, chase
    if (dist <= enemy.spec->detection_radius) {
        // Apply seeking weights toward player
        enemies::atoms::apply_seek_weights(enemy, player_pos, 1.0f);
        return enemies::BehaviorResult::Running;
    }
    
    // Player not in range
    return enemies::BehaviorResult::Failed;
}

// Improved chase player with obstacle avoidance
enemies::BehaviorResult chase_player_smart(enemies::EnemyRuntime& enemy, float dt) {
    // Get player position
    Vector2 player_pos = get_player_position();
    
    // Calculate distance to player
    float dist = Vector2Distance(enemy.position, player_pos);
    
    // If player is within detection radius, chase with obstacle avoidance
    if (dist <= enemy.spec->detection_radius) {
        // Use the enhanced obstacle avoidance implementation
        return enhanced_obstacle_avoidance(enemy, dt);
    }
    
    // Player not in range
    return enemies::BehaviorResult::Failed;
}

enemies::BehaviorResult attack_player(enemies::EnemyRuntime& enemy, float dt) {
    // Get player position
    Vector2 player_pos = get_player_position();
    
    // Forward to melee attack implementation
    return enemies::atoms::attack_melee(enemy, player_pos, dt);
}

// Function renamed to avoid overloading with different return types
bool attack_player_with_adapter(EnemyRuntime& enemy, Vector2 player_pos, float dt) {
    // Get access to the attack module
    auto& attack = enemy.attack;
    
    // If already on cooldown, update timer
    if (!attack.can_attack) {
        attack.timer += dt;
        if (attack.timer >= attack.cooldown) {
            attack.can_attack = true;
        }
        return false;
    }
    
    // Calculate distance to player
    float dx = player_pos.x - enemy.position.x;
    float dy = player_pos.y - enemy.position.y;
    float dist_to_player = sqrtf(dx * dx + dy * dy);
    
    // If within attack range, try to attack
    if (dist_to_player <= attack.attack_radius) {
        // Call the common attack_melee function which will create the attack rectangle
        enemies::BehaviorResult result = enemies::atoms::attack_melee(enemy, player_pos, dt);
        
        // If the attack started or is in progress, apply damage (only once per attack)
        if (result == enemies::BehaviorResult::Running && 
            enemy.attack_melee.attacking && 
            !enemy.attack_melee.damage_applied) {
            
            // Calculate normalized direction to player for knockback
            Vector2 attack_dir = {dx, dy};
            float len = sqrtf(attack_dir.x * attack_dir.x + attack_dir.y * attack_dir.y);
            if (len > 0) {
                attack_dir.x /= len;
                attack_dir.y /= len;
            }
            
            // Apply damage using the core entity adapter
            core::entity::damage_player(enemy.spec->dmg, attack_dir);
            
            // Mark damage as applied for this attack sequence
            enemy.attack_melee.damage_applied = true;
            
            // Log that damage was applied
            TraceLog(LOG_INFO, "Damage applied to player: %d", enemy.spec->dmg);
        }
        
        return true; // Attack was attempted
    }
    
    return false; // Couldn't attack
}

void toggle_debug_visualization() {
    show_debug = !show_debug;
}

void set_debug_visualization(bool enabled) {
    show_debug = enabled;
}

bool is_debug_visualization_enabled() {
    return show_debug;
}

void toggle_steering_debug() {
    show_steering_debug = !show_steering_debug;
}

bool is_steering_debug_enabled() {
    return show_steering_debug;
}

void toggle_obstacle_avoidance_visualization() {
    g_show_obstacle_avoidance = !g_show_obstacle_avoidance;
}

} // namespace atoms
} // namespace enemy 