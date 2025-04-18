/// enemy_state.cpp — Enemy state management atom implementation

#include "enemy_state.hpp"
#include "behavior_atoms.hpp"
#include "../enemy_slime.hpp"
#include "../../world/world.hpp"
#include <algorithm>
#include <cstdlib>
#include <random>

namespace enemy {
namespace atoms {

// Storage for all active enemy instances
static std::vector<EnemyRuntime> enemies;

// Slime specifications according to WORLDBUILDING.MD
static const EnemyStats SLIME_SPEC = {
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

void init_enemy_state() {
    // Clear any existing enemies
    enemies.clear();
    
    // Set random seed for reproducible behavior
    srand(42);
}

// PERF: ~0.05-0.5ms depending on enemy count
void update_enemy_states(float dt) {
    for (auto& enemy : enemies) {
        if (!enemy.active) continue;
        
        // Store original position for collision resolution and movement detection
        Vector2 original_pos = enemy.position;
        Vector2 movement = {0, 0};
        
        // Run behavior atoms
        bool acted = false;
        
        // First try attack_player behavior if available
        if (std::find(enemy.spec->behaviors.begin(), enemy.spec->behaviors.end(), 
                    BehaviorAtom::attack_player) != enemy.spec->behaviors.end()) {
            BehaviorResult result = atoms::attack_player(enemy, dt);
            if (result != BehaviorResult::Failed) {
                acted = true;
            }
        }
        
        // If not attacking, try chase_player behavior
        if (!acted && std::find(enemy.spec->behaviors.begin(), enemy.spec->behaviors.end(), 
                             BehaviorAtom::chase_player) != enemy.spec->behaviors.end()) {
            BehaviorResult result = atoms::chase_player(enemy, dt);
            if (result == BehaviorResult::Running) {
                acted = true;
            }
        }
        
        // If not chasing or attacking, try wander_random
        if (!acted && std::find(enemy.spec->behaviors.begin(), enemy.spec->behaviors.end(), 
                              BehaviorAtom::wander_random) != enemy.spec->behaviors.end()) {
            atoms::wander_random(enemy, dt);
        }
        
        // Reset enemy color if not attacking (usually red during attack)
        if (!enemy.attack.attacking) {
            enemy.color = WHITE; // Changed to WHITE for sprite rendering
        } else {
            // Use a more subtle red tint that works better with sprites
            enemy.color = {255, 150, 150, 255}; // Light red tint instead of pure RED
        }
        
        // Check world boundaries - using proper world bounds instead of screen bounds
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
    
    // Remove dead enemies (cleanup will be called separately)
}

void add_enemy(const EnemyRuntime& enemy) {
    enemies.push_back(enemy);
}

const std::vector<EnemyRuntime>& get_enemies() {
    return enemies;
}

void cleanup_inactive_enemies() {
    // Remove dead enemies (move to end and erase)
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(), 
            [](const EnemyRuntime& enemy) { return !enemy.active || enemy.hp <= 0; }),
        enemies.end()
    );
}

int get_active_enemy_count() {
    return enemies.size();
}

bool apply_damage_at(const Rectangle& hit_rect, const Hit& hit) {
    // Check each enemy for collision with the hit rectangle
    for (auto& enemy : enemies) {
        if (!enemy.active || enemy.hp <= 0) continue;
        
        // Check if hit rectangle intersects with enemy collision rectangle
        if (CheckCollisionRecs(hit_rect, enemy.collision_rect)) {
            // Enemy takes damage
            enemy.hp -= hit.dmg;
            
            // Apply knockback (if any)
            enemy.position.x += hit.knockback.x;
            enemy.position.y += hit.knockback.y;
            
            // Update collision rectangle
            enemy.collision_rect.x = enemy.position.x - enemy.spec->size.x/2;
            enemy.collision_rect.y = enemy.position.y - enemy.spec->size.y/2;
            
            // Check if enemy is killed
            if (enemy.hp <= 0) {
                // Enemy is dead - check for drops
                // This logic will move to a separate atom later
                TraceLog(LOG_INFO, "Enemy dropped: Heart");
                TraceLog(LOG_INFO, "Enemy dropped: Coin");
                
                // Mark as inactive - will be cleaned up in cleanup_inactive_enemies
                enemy.active = false;
            }
            
            return true; // Hit successful
        }
    }
    
    return false; // No hit
}

void clear_enemies() {
    enemies.clear();
}

const EnemyStats& get_slime_spec() {
    return SLIME_SPEC;
}

} // namespace atoms
} // namespace enemy 