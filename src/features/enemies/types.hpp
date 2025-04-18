/// types.hpp — Common enemy data structures for all enemy slices
#pragma once
#include <raylib.h>
#include <array>
#include <vector>

namespace enemies {

/// Enemy type identifiers as defined in WORLDBUILDING.MD §5.3
enum class EnemyID {
    // Forest region enemies
    FOR_SLIME,
    FOR_BOAR,
    // Cave region enemies 
    CAV_BAT,
    // Desert region enemies
    DES_SCARAB,
    // Snow region enemies
    SNW_WOLF,
    // Ruins region enemies
    RUN_DRONE
};

/// Types of items an enemy can drop
enum class DropType {
    Heart,
    Coin,
    Shard
};

/// Probability of a specific drop
struct DropChance {
    DropType type;
    int chance; // Percentage (0-100)
};

/// Behavior building blocks that can be composed
enum class BehaviorAtom {
    wander_random,
    chase_player,
    attack_player,
    ranged_shoot,
    charge_dash,
    armor_gate,
    dead_poof
};

/// Direction the enemy is facing (for animations)
enum class Facing {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

/// Static data for an enemy type as defined in WORLDBUILDING.MD §5.1
struct EnemyStats {
    EnemyID id;                                   // Unique identifier for this enemy type
    Vector2 size;                                 // Size in pixels (collision dimensions)
    int hp;                                       // Hit points
    int dmg;                                      // Damage per hit
    float speed;                                  // Movement speed in pixels/second
    std::vector<BehaviorAtom> behaviors;          // Composed behavior atoms
    std::array<DropChance, 2> drops;              // Drop chances
    int animation_frames;                         // Number of animation frames
    float radius;                                 // Collision radius
    float width;                                  // Visual width
    float height;                                 // Visual height
    float detection_radius;                       // How far enemy can detect player
    float attack_radius;                          // Attack range
    float attack_cooldown;                        // Time between attacks
};

/// Damage application structure
struct Hit {
    int dmg;                                      // Damage amount
    Vector2 knockback;                            // Knockback force
    enum class Type { Melee, Arrow, Fire, Ice } type; // Damage type
};

/// Wander behavior state
struct WanderRandom {
    float radius = 100.0f;                        // Maximum wander distance
    Vector2 target = {0.0f, 0.0f};                // Current target position
    bool has_target = false;                      // Whether we have a valid target
    float idle_time = 1.0f;                       // How long to wait between movements
    float current_timer = 0.0f;                   // Timer for current state
};

/// Player chase behavior state
struct ChasePlayer {
    float detection_radius = 200.0f;              // Distance at which player is detected
    bool chasing = false;                         // Whether currently chasing player
};

/// Attack behavior state
struct AttackPlayer {
    float attack_radius = 50.0f;                  // Maximum attack distance
    float cooldown = 1.2f;                        // Time between attacks
    float timer = 0.0f;                           // Current cooldown timer
    bool can_attack = true;                       // Whether can currently attack
    bool attacking = false;                       // Whether currently attacking
};

/// Result of a behavior atom execution
enum class BehaviorResult {
    Running,   // Still running this behavior
    Completed, // Successfully completed behavior
    Failed     // Failed to perform behavior
};

/// Runtime state for an enemy instance
struct EnemyRuntime {
    const EnemyStats* spec;                       // Reference to static enemy data
    Vector2 position;                             // Current position
    int hp;                                       // Current health
    Rectangle collision_rect;                     // Collision rectangle
    Color color;                                  // Rendering color/tint
    Facing facing;                                // Direction enemy is facing
    WanderRandom wander;                          // Wander behavior state
    ChasePlayer chase;                            // Chase behavior state
    AttackPlayer attack;                          // Attack behavior state
    bool active;                                  // Whether this enemy is active
    float anim_timer;                             // Animation timer
    int anim_frame;                               // Current animation frame
    bool is_moving;                               // Whether enemy is currently moving
    
    // Constructor to initialize an enemy instance
    EnemyRuntime(const EnemyStats& spec_ref, Vector2 pos);
};

/// Spawn request data from the level loader
struct EnemySpawnRequest {
    EnemyID id;                                   // Enemy type to spawn
    Vector2 position;                             // World position to spawn at
    bool respawnable;                             // Whether enemy should respawn
};

} // namespace enemies 