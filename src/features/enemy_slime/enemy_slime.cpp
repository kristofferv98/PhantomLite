/// enemy_slime.cpp — Forest Slime enemy implementation

#include "enemy_slime.hpp"
#include "atoms/behavior_atoms.hpp"
#include "atoms/enemy_state.hpp"
#include "atoms/enemy_renderer.hpp"
#include "atoms/enemy_combat.hpp"
#include "atoms/enemy_spawner.hpp"
#include "../player/player.hpp"
#include "../world/world.hpp"
// Include raylib directly instead of the missing raylib_ext.hpp
#include "raylib.h"
#include <algorithm>
#include <cmath>
#include <random>
#include <vector>

// Get player position from player module instead of screen center
Vector2 get_player_position() {
    return player::get_position();
}

namespace enemy {

// Slime specifications according to WORLDBUILDING.MD
static const EnemySpec SLIME_SPEC = {
    .id = EnemyID::FOR_SLIME,
    .size = {32.0f, 32.0f},  // 1x1 tile (32x32 pixels)
    .hp = 2,                 // 2 pips of health
    .dmg = 1,                // 1 pip of damage
    .speed = 60.0f,          // 60 pixels per second
    .behaviors = {BehaviorAtom::wander_random, BehaviorAtom::chase_player, BehaviorAtom::attack_player},
    .drops = {DropChance{DropType::Heart, 30}, DropChance{DropType::Coin, 70}},
    .animation_frames = 2,    // Slime has 2 animation frames
    .radius = 16.0f,         // Collision radius (half of size)
    .width = 32.0f,          // Sprite width
    .height = 32.0f,         // Sprite height
    .detection_radius = 200.0f, // Detection radius for chase behavior
    .attack_radius = 50.0f,   // Attack radius
    .attack_cooldown = 1.2f   // Attack cooldown in seconds
};

// Storage for all active slime instances
static std::vector<EnemyInstance> slimes;

// Animation frame time
static const float ANIMATION_FRAME_TIME = 0.25f; // Seconds per frame

// Texture for slime sprites
static Texture2D slime_texture;
static Texture2D slime_squash_texture;

// Debug visualization flag
static bool show_debug = false;

// Constructor implementation for EnemyInstance
EnemyInstance::EnemyInstance(const EnemySpec& spec_ref, Vector2 pos)
    : spec(&spec_ref), 
      position(pos), 
      hp(spec_ref.hp), 
      collision_rect({pos.x - spec_ref.size.x/2, pos.y - spec_ref.size.y/2, spec_ref.size.x, spec_ref.size.y}),
      color(GREEN),
      wander(100.0f, 1.0f),  // Wander within 100px, idle for 1s
      chase(200.0f),         // Detect player within 200px
      attack(50.0f, 1.2f),   // Attack within 50px, cooldown 1.2s
      active(true),
      anim_timer(0.0f),
      anim_frame(0),
      is_moving(false) {}

// Initialize the enemy system
void init_enemies() {
    atoms::init_enemy_state();
    atoms::init_renderer();
    atoms::init_combat();
    atoms::init_spawner();
}

// PERF: ~0.05-0.5ms depending on enemy count
void update_enemies(float dt) {
    atoms::update_enemy_states(dt);
}

// Demo function to spawn multiple slimes and show behavior
void spawn_demo_slimes(int count) {
    atoms::spawn_demo_slimes(count);
}

void toggle_debug_info() {
    atoms::toggle_debug_visualization();
}

void toggle_debug() {
    toggle_debug_info();
}

void set_debug(bool enabled) {
    atoms::set_debug_visualization(enabled);
}

bool is_debug_enabled() {
    return atoms::is_debug_visualization_enabled();
}

int get_enemy_count() {
    return atoms::get_active_enemy_count();
}

// Enhanced rendering with sprite images and debug visualization
void render_enemies() {
    atoms::render_enemies();
}

void spawn_slime(Vector2 position) {
    atoms::spawn_slime_at(position);
}

bool hit_enemy_at(const Rectangle& hit_rect, const Hit& hit) {
    return atoms::hit_enemy_at(hit_rect, hit);
}

void cleanup_enemies() {
    atoms::cleanup_renderer();
    atoms::clear_enemies();
}

const EnemyStats& get_slime_spec() {
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

} // namespace enemy 