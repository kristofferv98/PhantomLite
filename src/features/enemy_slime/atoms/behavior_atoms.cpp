/// behavior_atoms.cpp — Implementation of enemy behavior building blocks

#include "behavior_atoms.hpp"
#include "../enemy_slime.hpp"
#include "../../player/player.hpp"
#include "../../enemies/behavior_atoms.hpp"
#include "shared/opensimplex2/opensimplex2.h"
#include <cmath>
#include <raymath.h>
#include <vector>

namespace enemy {
namespace atoms {

// Debug flags
static bool show_debug = false;
static bool show_steering_debug = false;

// Global noise generator for consistent behavior
static opensimplex2::Noise noise_gen(42); // Fixed seed for reproducible behavior

Vector2 get_player_position() {
    return player::get_position();
}

// PERF: ~0.03-0.08ms per enemy
enemies::BehaviorResult wander_noise(enemies::EnemyRuntime& enemy, float dt) {
    // Update noise sampling position
    enemy.wander_noise.noise_offset_x += dt * enemy.wander_noise.sway_speed;
    enemy.wander_noise.noise_offset_y += dt * enemy.wander_noise.sway_speed * 0.7f; // Different frequency for y
    
    // Sample Simplex noise at the current position (3 octaves)
    float noise_x = noise_gen.noise2(
        enemy.wander_noise.noise_offset_x, 
        enemy.wander_noise.noise_offset_y + 500.0f, // offset y for different sampling space
        3, 0.5f);
    
    float noise_y = noise_gen.noise2(
        enemy.wander_noise.noise_offset_x + 500.0f, 
        enemy.wander_noise.noise_offset_y,
        3, 0.5f);
    
    // Map noise [-1,1] to full circle direction
    float angle = (noise_x * 2.0f - 1.0f) * PI;
    
    // Create a direction vector from noise
    Vector2 wander_dir = { cosf(angle), sinf(angle) };
    
    // Get distance from spawn point
    Vector2 to_spawn = {
        enemy.wander_noise.spawn_point.x - enemy.position.x,
        enemy.wander_noise.spawn_point.y - enemy.position.y
    };
    float dist_to_spawn = sqrtf(to_spawn.x * to_spawn.x + to_spawn.y * to_spawn.y);
    
    // If too far from spawn point, blend in a return vector
    if (dist_to_spawn > enemy.wander_noise.radius) {
        // Normalize to_spawn
        float spawn_length = sqrtf(to_spawn.x * to_spawn.x + to_spawn.y * to_spawn.y);
        if (spawn_length > 0) {
            to_spawn.x /= spawn_length;
            to_spawn.y /= spawn_length;
        }
        
        // Blend based on how far beyond radius
        float blend = fminf((dist_to_spawn - enemy.wander_noise.radius) / 50.0f, 1.0f);
        wander_dir.x = wander_dir.x * (1.0f - blend) + to_spawn.x * blend;
        wander_dir.y = wander_dir.y * (1.0f - blend) + to_spawn.y * blend;
        
        // Renormalize
        float len = sqrtf(wander_dir.x * wander_dir.x + wander_dir.y * wander_dir.y);
        if (len > 0) {
            wander_dir.x /= len;
            wander_dir.y /= len;
        }
    }
    
    // Apply steering weights based on wander direction
    enemies::atoms::apply_direction_weights(enemy, wander_dir, 0.5f); // Lower priority for wandering
    
    return enemies::BehaviorResult::Running;
}

// PERF: ~0.01-0.02ms per enemy
enemies::BehaviorResult chase_player(enemies::EnemyRuntime& enemy, float dt) {
    // Get player position
    Vector2 player_pos = get_player_position();
    
    // Calculate distance to player
    float dist = Vector2Distance(enemy.position, player_pos);
    
    // If player is within detection radius, chase
    if (dist <= enemy.spec->detection_radius) {
        // Mark as chasing for animation/state
        enemy.chase.chasing = true;
        
        // Apply seeking weights toward player
        enemies::atoms::apply_seek_weights(enemy, player_pos, 1.0f);
        return enemies::BehaviorResult::Running;
    }
    
    // Player not in range
    enemy.chase.chasing = false;
    return enemies::BehaviorResult::Failed;
}

// PERF: ~0.02-0.03ms per enemy
enemies::BehaviorResult strafe_player(enemies::EnemyRuntime& enemy, float dt) {
    // Get player position
    Vector2 player_pos = get_player_position();
    
    // Calculate distance to player
    float dist = Vector2Distance(enemy.position, player_pos);
    
    // If player is within detection radius but outside melee range, strafe
    if (dist <= enemy.spec->detection_radius && dist > enemy.spec->attack_radius * 1.2f) {
        // Apply strafing weights for orbiting the player
        // First parameter: enemy instance
        // Second parameter: orbit target position
        // Third parameter: orbit direction (1=clockwise, -1=counterclockwise)
        // Fourth parameter: strength of the orbit behavior
        enemies::atoms::apply_strafe_weights(
            enemy, 
            player_pos, 
            enemy.strafe_target.direction, 
            enemy.strafe_target.orbit_gain
        );
        return enemies::BehaviorResult::Running;
    }
    
    return enemies::BehaviorResult::Failed;
}

// PERF: ~0.02-0.04ms per enemy
enemies::BehaviorResult separate_from_allies(enemies::EnemyRuntime& enemy, float dt) {
    // Get all enemies from the state
    const auto& all_enemies = get_enemies();
    
    // Apply separation weights
    enemies::atoms::apply_separation_weights(
        enemy,
        all_enemies,
        enemy.separate_allies.desired_spacing,
        enemy.separate_allies.separation_gain
    );
    
    return enemies::BehaviorResult::Running;
}

// PERF: ~0.01-0.02ms per enemy
enemies::BehaviorResult attack_player(enemies::EnemyRuntime& enemy, float dt) {
    // Get player position
    Vector2 player_pos = get_player_position();
    
    // Forward to melee attack implementation
    return enemies::atoms::attack_melee(enemy, player_pos, dt);
}

// PERF: ~0.02-0.04ms per enemy
enemies::BehaviorResult avoid_obstacles(enemies::EnemyRuntime& enemy, float dt) {
    // Apply obstacle avoidance weights
    enemies::atoms::apply_obstacle_avoidance_weights(
        enemy,
        enemy.avoid_obstacle.lookahead_px,
        enemy.avoid_obstacle.avoidance_gain
    );
    
    return enemies::BehaviorResult::Running;
}

// The following functions are implemented in enemy_renderer.cpp
// to avoid duplicate symbol errors:
// - toggle_debug_visualization()
// - set_debug_visualization(bool)
// - is_debug_visualization_enabled()
// - toggle_steering_debug()
// - is_steering_debug_enabled()

} // namespace atoms
} // namespace enemy 