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
    // Delegate to the standard implementation in enemies namespace for consistency
    return enemies::atoms::wander_noise(enemy, dt);
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