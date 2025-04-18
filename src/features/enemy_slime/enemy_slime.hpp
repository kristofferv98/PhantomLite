/// enemy_slime.hpp — Forest Slime enemy (public API)

#pragma once

#include "raylib.h"
#include "atoms/behavior_atoms.hpp"
#include <vector>
#include <array>

namespace enemy {

/// Enemy types from WORLDBUILDING.MD
enum class EnemyID {
    FOR_SLIME,    ///< Forest slime
    FOR_BOAR,     ///< Forest boar
    CAV_BAT,      ///< Cave bat
    DES_SCARAB,   ///< Desert scarab
    SNW_WOLF,     ///< Snow wolf
    RUN_DRONE     ///< Ruins drone
};

/// Item drop types
enum class DropType {
    Heart,      ///< Health pickup
    Coin        ///< Currency
};

/// Drop chance entry
struct DropChance {
    DropType type;  ///< What to drop
    int chance;     ///< Percent chance (0-100)
};

/// Behavior atoms used by this enemy
enum class BehaviorAtom {
    wander_random,
    chase_player,
    attack_player,  ///< Attack player when in range
    ranged_shoot,
    charge_dash,
    armor_gate,
    dead_poof
};

/// Enemy specification - static data for an enemy type
struct EnemySpec {
    EnemyID id;                       ///< Unique ID
    Vector2 size;                     ///< Size in pixels (32×32 for 1×1 tile)
    int hp;                           ///< Health in pips
    int dmg;                          ///< Damage dealt in pips
    float speed;                      ///< Movement speed in pixels/sec
    std::array<BehaviorAtom, 3> behaviors; ///< Behavior atoms used
    std::array<DropChance, 2> drops;  ///< Possible drops
    int animation_frames;             ///< Number of animation frames
};

/// Enemy instance - dynamic data for a specific enemy
struct EnemyInstance {
    const EnemySpec* spec;    ///< Pointer to static data
    Vector2 position;         ///< Current position
    int hp;                   ///< Current health
    Rectangle collision_rect; ///< Collision rectangle
    Color color;              ///< Color (for simplified rendering)
    WanderRandom wander;      ///< Wander behavior data
    ChasePlayer chase;        ///< Chase behavior data
    AttackPlayer attack;      ///< Attack behavior data
    bool active;              ///< Whether this enemy is active
    float anim_timer;         ///< Animation timer
    int anim_frame;           ///< Current animation frame
    bool is_moving;           ///< Whether the enemy is moving
    
    EnemyInstance(const EnemySpec& spec_ref, Vector2 pos);
};

/// Hit information for damage calculation
struct Hit {
    int dmg;            ///< Damage in pips
    Vector2 knockback;  ///< Knockback force
    enum class Type { Melee, Arrow, Fire, Ice } type; ///< Damage type
};

/// Initialize the enemy system
void init_enemies();

/// Update all active enemies
void update_enemies(float dt);

/// Render all active enemies
void render_enemies();

/// Spawn a slime at the given position
void spawn_slime(Vector2 position);

/// Spawn multiple slimes in random positions for demo purposes
void spawn_demo_slimes(int count);

/// Toggle visibility of debug information
void toggle_debug_info();

/// Process a hit on an enemy at the given position, returns true if hit successful
bool hit_enemy_at(const Rectangle& hit_rect, const Hit& hit);

/// Cleanup enemy resources
void cleanup_enemies();

/// Get the slime enemy specification
const EnemySpec& get_slime_spec();

} // namespace enemy 