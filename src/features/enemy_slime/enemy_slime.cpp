/// enemy_slime.cpp — Forest Slime enemy implementation

#include "enemy_slime.hpp"
#include "atoms/behavior_atoms.hpp"
#include "atoms/enemy_state.hpp"
#include "atoms/enemy_renderer.hpp"
#include "atoms/enemy_spawning.hpp"
#include "../player/player.hpp"
#include "../world/world.hpp"
#include "core/public/entity.hpp"
// Include raylib directly instead of the missing raylib_ext.hpp
#include "raylib.h"
#include <algorithm>
#include <cmath>
#include <random>
#include <vector>

// Helper function for calculating distance
float calculate_distance(const Vector2& a, const Vector2& b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return sqrtf(dx * dx + dy * dy);
}

// Get player position from player module instead of screen center
Vector2 get_player_position() {
    return player::get_position();
}

namespace enemy {

// Debug visualization flag
static bool show_debug = false;
static bool show_steering_debug = false;

// Initialize the enemy system
void init_enemies() {
    atoms::init_enemy_state();
    atoms::init_renderer();
    atoms::init_spawning();
}

// PERF: ~0.05-0.5ms depending on enemy count
void update_enemies(float dt) {
    atoms::update_enemy_states(dt);
}

// Demo function to spawn multiple slimes and show behavior
void spawn_demo_slimes(int count) {
    // Use the atomic function for spawning multiple enemies
    Vector2 player_pos = player::get_position();
    std::vector<enemies::EnemyRuntime> enemies = atoms::get_enemies();
    
    // Spawn new enemies around the player
    atoms::spawn_enemies_around_player(player_pos, 10.0f, enemies, count);
    
    // Clear existing enemies and add the new ones
    atoms::clear_enemies();
    for (const auto& enemy : enemies) {
        atoms::add_enemy(enemy);
    }
}

void toggle_debug_info() {
    atoms::toggle_debug_visualization();
    show_debug = atoms::is_debug_visualization_enabled();
}

void toggle_steering_debug() {
    atoms::toggle_steering_debug();
    show_steering_debug = atoms::is_steering_debug_enabled();
}

void toggle_debug() {
    toggle_debug_info();
}

void set_debug(bool enabled) {
    atoms::set_debug_visualization(enabled);
    show_debug = enabled;
}

bool is_debug_enabled() {
    return atoms::is_debug_visualization_enabled();
}

bool is_steering_debug_enabled() {
    return atoms::is_steering_debug_enabled();
}

int get_enemy_count() {
    return atoms::get_active_enemy_count();
}

// Enhanced rendering with sprite images and debug visualization
void render_enemies() {
    atoms::render_enemies();
}

void spawn_slime(Vector2 position) {
    // Create a slime at the given position using spawn_enemy
    enemies::EnemyRuntime slime = atoms::spawn_enemy(position, enemies::EnemyType::SLIME_SMALL);
    atoms::add_enemy(slime);
}

bool hit_enemy_at(const Rectangle& hit_rect, const enemies::Hit& hit) {
    // Use apply_damage_at directly from enemy_state
    return atoms::apply_damage_at(hit_rect, hit);
}

void cleanup_enemies() {
    atoms::cleanup_renderer();
    atoms::clear_enemies();
    atoms::cleanup_spawning();
}

const enemies::EnemyStats& get_slime_spec() {
    return atoms::get_slime_spec();
}

// Legacy API implementation
void init() {
    init_enemies();
}

void update(float delta_time) {
    update_enemies(delta_time);
}

void render() {
    render_enemies();
}

void cleanup() {
    cleanup_enemies();
}

bool check_player_collision(Vector2 position, float radius, int* enemy_id) {
    // Implementation maintained for backward compatibility
    
    // Get all enemies
    const auto& enemies = atoms::get_enemies();
    
    // Check collision with each enemy
    for (size_t i = 0; i < enemies.size(); i++) {
        const auto& enemy = enemies[i];
        if (!enemy.active) continue;
        
        // Check circle collision
        float dist = calculate_distance(position, enemy.position);
        if (dist < radius + enemy.spec->radius) {
            // Collision detected
            if (enemy_id) *enemy_id = static_cast<int>(i);
            return true;
        }
    }
    
    return false;
}

void take_damage(int enemy_id, int damage) {
    // Implementation maintained for backward compatibility
    
    // Get mutable reference to enemies
    auto& enemies = atoms::get_enemies_mutable();
    
    // Validate index
    if (enemy_id < 0 || enemy_id >= static_cast<int>(enemies.size())) {
        return;
    }
    
    // Get player position for knockback direction
    Vector2 player_pos = player::get_position();
    
    // Apply damage
    enemies::Hit hit;
    hit.dmg = damage;
    hit.knockback_magnitude = 25.0f; // Medium strength knockback
    hit.source_position = player_pos; // Knockback away from player
    hit.type = enemies::Hit::Type::Melee;
    enemies[enemy_id].on_hit(hit);
}

} // namespace enemy 