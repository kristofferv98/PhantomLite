/// behavior_atoms.cpp — Implementation of enemy behavior building blocks

#include "behavior_atoms.hpp"
#include "../enemy_slime.hpp"
#include "../../player/player.hpp"
#include "../../enemies/behavior_atoms.hpp"
#include <cmath>
#include <raymath.h>

namespace enemy {
namespace atoms {

// Debug flags
static bool show_debug = false;
static bool show_steering_debug = false;

Vector2 get_player_position() {
    return player::get_position();
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

enemies::BehaviorResult attack_player(enemies::EnemyRuntime& enemy, float dt) {
    // Get player position
    Vector2 player_pos = get_player_position();
    
    // Forward to melee attack implementation
    return enemies::atoms::attack_melee(enemy, player_pos, dt);
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

} // namespace atoms
} // namespace enemy 