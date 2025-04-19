/// types.hpp — Common enemy data structures for all enemy slices
#pragma once
#include <raylib.h>
#include <array>
#include <vector>
#include <cmath>
#include <string>

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

/// Enemy type enum for determining appearance
enum class EnemyType {
    SLIME_SMALL,
    SLIME_MEDIUM,
    SLIME_LARGE,
    BOAR,
    BAT,
    SCARAB,
    WOLF,
    DRONE
};

/// Behavior building blocks that can be composed
enum class BehaviorAtom {
    // Basic behaviors
    wander_random,     // Original simple wandering
    chase_player,      // Simple player chasing
    attack_player,     // Simple player attacking
    
    // Advanced steering behaviors
    wander_noise,      // Wander using Perlin noise for smoother paths
    seek_target,       // Move directly toward a target
    strafe_target,     // Orbit around a target
    separate_allies,   // Maintain distance from other enemies
    avoid_obstacle,    // Avoid obstacles by raycast detection
    context_steer,     // Combined steering behavior
    
    // Combat behaviors
    charge_dash,       // Wind up and dash in a direction
    ranged_shoot,      // Fire projectiles at a distance
    attack_melee,      // Attack at close range
    
    // Effects
    armor_gate,        // Ignore damage unless a weak spot is hit
    dead_poof          // Visual effect on death
};

/// Direction the enemy is facing (for animations)
enum class Facing {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

/// Behavior flags for enabling specific behaviors
enum class BehaviorFlags {
    NONE              = 0,
    WANDER_NOISE      = 1 << 0,
    BASIC_CHASE       = 1 << 1,
    ADVANCED_CHASE    = 1 << 2,
    STRAFE_TARGET     = 1 << 3,
    SEPARATE_ALLIES   = 1 << 4,
    AVOID_OBSTACLES   = 1 << 5,
    CHARGE_DASH       = 1 << 6,
    RANGED_ATTACK     = 1 << 7,
    MELEE_ATTACK      = 1 << 8,
    ARMOR_GATE        = 1 << 9
};

/// Enable bitwise operations on BehaviorFlags
inline BehaviorFlags operator|(BehaviorFlags a, BehaviorFlags b) {
    return static_cast<BehaviorFlags>(static_cast<int>(a) | static_cast<int>(b));
}

inline BehaviorFlags operator&(BehaviorFlags a, BehaviorFlags b) {
    return static_cast<BehaviorFlags>(static_cast<int>(a) & static_cast<int>(b));
}

/// Static data for an enemy type as defined in WORLDBUILDING.MD §5.1
struct EnemyStats {
    EnemyID id;                                   // Unique identifier for this enemy type
    EnemyType type;                               // Type of enemy (visual & behavior)
    std::string name;                             // Descriptive name
    Vector2 size;                                 // Size in pixels (collision dimensions)
    int hp;                                       // Hit points
    int dmg;                                      // Damage per hit
    float speed;                                  // Movement speed in pixels/second
    std::vector<BehaviorAtom> behaviors;          // Composed behavior atoms
    std::vector<DropChance> drops;                // Drop chances
    int animation_frames;                         // Number of animation frames
    float radius;                                 // Collision radius
    float width;                                  // Visual width
    float height;                                 // Visual height
    float detection_radius;                       // How far enemy can detect player
    float attack_radius;                          // Attack range
    float attack_cooldown;                        // Time between attacks
    BehaviorFlags behavior_flags = BehaviorFlags::NONE; // Behavior flags
};

/// Damage application structure
struct Hit {
    int dmg;                                      // Damage amount
    float knockback_magnitude;                    // NEW: Base magnitude of knockback force
    Vector2 source_position;                      // NEW: Position where the hit originated from
    enum class Type { 
        Melee, 
        Arrow, 
        Fire, 
        Ice,
        Pierce,  // Added new damage type
        Magic    // Added new damage type
    } type;      // Damage type
};

/// Wander behavior state (original simple version)
struct WanderRandom {
    float radius = 100.0f;                        // Maximum wander distance
    Vector2 target = {0.0f, 0.0f};                // Current target position
    bool has_target = false;                      // Whether we have a valid target
    float idle_time = 1.0f;                       // How long to wait between movements
    float current_timer = 0.0f;                   // Timer for current state
};

/// Player chase behavior state (original simple version)
struct ChasePlayer {
    float detection_radius = 200.0f;              // Distance at which player is detected
    bool chasing = false;                         // Whether currently chasing player
};

/// Attack behavior state (original simple version)
struct AttackPlayer {
    float attack_radius = 50.0f;                  // Maximum attack distance
    float cooldown = 1.2f;                        // Time between attacks
    float timer = 0.0f;                           // Current cooldown timer
    bool can_attack = true;                       // Whether can currently attack
    bool attacking = false;                       // Whether currently attacking
};

/// NEW: Advanced wander behavior using noise for smoother paths
struct WanderNoise {
    float radius = 100.0f;                        // Maximum wander distance
    float sway_speed = 0.5f;                      // How quickly to change direction
    Vector2 spawn_point = {0.0f, 0.0f};           // Initial spawn location
    float noise_offset_x = 0.0f;                  // Noise sampling offset X
    float noise_offset_y = 0.0f;                  // Noise sampling offset Y
};

/// NEW: Seek target behavior (move toward a target point)
struct SeekTarget {
    float preferred_dist = 0.0f;                  // Preferred distance to maintain
    bool active = false;                          // Whether currently seeking
    float seek_gain = 1.0f;                       // How strongly to seek
};

/// NEW: Strafe target behavior (orbit around a point)
struct StrafeTarget {
    float orbit_radius = 100.0f;                  // Ideal distance to orbit
    int direction = 1;                            // 1 for clockwise, -1 for counterclockwise
    bool active = false;                          // Whether currently strafing
    float orbit_gain = 1.0f;                      // How strongly to orbit
};

/// NEW: Separate from allies (maintain spacing between enemies)
struct SeparateAllies {
    float desired_spacing = 50.0f;                // Minimum distance between enemies
    float separation_gain = 1.0f;                 // How strongly to separate
};

/// NEW: Avoid obstacles using raycasts
struct AvoidObstacle {
    float lookahead_px = 100.0f;                  // How far ahead to check for obstacles
    float avoidance_gain = 2.0f;                  // How strongly to avoid obstacles
};

/// NEW: Charge and dash attack
struct ChargeDash {
    enum class State {
        Idle,
        Charging,
        Dashing,
        Cooldown
    };
    
    State state = State::Idle;                    // Current dash state
    float charge_timer = 0.0f;                    // Windup timer
    float charge_duration = 0.5f;                 // How long to wind up
    float dash_timer = 0.0f;                      // Dash duration timer
    float dash_duration = 0.3f;                   // How long to dash
    float cooldown_timer = 0.0f;                  // Cooldown timer
    float cooldown_duration = 2.0f;               // How long to wait between dashes
    float dash_speed = 300.0f;                    // Speed multiplier while dashing
    Vector2 dash_direction = {0.0f, 0.0f};        // Direction to dash
};

/// NEW: Ranged attack behavior
struct RangedShoot {
    float cooldown = 2.0f;                        // Time between shots
    float timer = 0.0f;                           // Current cooldown timer
    bool can_fire = true;                         // Whether can currently fire
    float projectile_speed = 200.0f;              // Speed of projectiles
    int projectile_damage = 1;                    // Damage dealt by projectiles
};

/// NEW: Melee attack behavior
struct AttackMelee {
    float reach = 40.0f;                          // Attack reach distance
    float cooldown = 1.0f;                        // Time between attacks
    float timer = 0.0f;                           // Current cooldown timer
    bool can_attack = true;                       // Whether can currently attack
    bool attacking = false;                       // Whether currently attacking
    float attack_duration = 0.3f;                 // Duration of attack animation
    float attack_timer = 0.0f;                    // Current attack animation timer
    bool damage_applied = false;                  // Track if damage has been applied during this attack
};

/// Result of a behavior atom execution
enum class BehaviorResult {
    Running,   // Still running this behavior
    Completed, // Successfully completed behavior
    Failed     // Failed to perform behavior
};

/// Runtime state for an enemy instance with 16-ray steering grid
struct EnemyRuntime {
    const EnemyStats* spec;                       // Reference to static enemy data
    Vector2 position;                             // Current position
    int hp;                                       // Current health
    Rectangle collision_rect;                     // Collision rectangle
    Color color;                                  // Rendering color/tint
    Facing facing;                                // Direction enemy is facing
    bool active;                                  // Whether this enemy is active
    float anim_timer;                             // Animation timer
    int anim_frame;                               // Current animation frame
    bool is_moving;                               // Whether enemy is currently moving
    
    // NEW: 16-ray steering grid for context steering
    static constexpr int NUM_RAYS = 16;           // Rays spread every 22.5 degrees
    std::array<float, NUM_RAYS> weights = {0};    // -1 (blocked) to 1 (desired)
    
    // Behavior state data (original + new)
    WanderRandom wander;                          // Original wander behavior
    ChasePlayer chase;                            // Original chase behavior
    AttackPlayer attack;                          // Original attack behavior
    
    // NEW: Advanced behavior state data
    WanderNoise wander_noise;                     // Noise-based wandering
    SeekTarget seek_target;                       // Target seeking
    StrafeTarget strafe_target;                   // Target strafing
    SeparateAllies separate_allies;               // Ally separation
    AvoidObstacle avoid_obstacle;                 // Obstacle avoidance
    ChargeDash charge_dash;                       // Charge and dash attack
    RangedShoot ranged_shoot;                     // Ranged attacks
    AttackMelee attack_melee;                     // Melee attacks
    
    // Constructor to initialize an enemy instance
    EnemyRuntime(const EnemyStats& spec_ref, Vector2 pos);
    
    // NEW: Helper methods
    bool is_alive() const { return hp > 0 && active; }
    void on_hit(const Hit& hit);
    
    // NEW: Reset all steering weights to zero
    void reset_weights() {
        for (int i = 0; i < NUM_RAYS; i++) {
            weights[i] = 0.0f;
        }
    }
    
    // NEW: Get the ray direction vector for a given ray index
    Vector2 get_ray_dir(int ray_index) const {
        float angle = ray_index * (2.0f * PI / NUM_RAYS);
        return { cosf(angle), sinf(angle) };
    }
    
    // NEW: Apply movement based on the best available direction
    void apply_steering_movement(float speed, float dt);
};

/// Spawn request data from the level loader
struct EnemySpawnRequest {
    EnemyID id;                                   // Enemy type to spawn
    Vector2 position;                             // World position to spawn at
    bool respawnable;                             // Whether enemy should respawn
};

} // namespace enemies 
