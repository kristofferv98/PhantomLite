/// enemy_slime.cpp — Forest Slime enemy implementation

#include "enemy_slime.hpp"
#include "atoms/behavior_atoms.hpp"
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
    .animation_frames = 2     // Slime has 2 animation frames
};

// Storage for all active slime instances
static std::vector<EnemyInstance> slimes;

// Animation frame time
static const float ANIMATION_FRAME_TIME = 0.25f; // Seconds per frame

// Texture for slime sprites
static Texture2D slime_texture;
static Texture2D slime_squash_texture;

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
    // Clear any existing enemies
    slimes.clear();
    
    // Load enemy textures
    slime_texture = LoadTexture("assets/sprites/for/slime.png");
    slime_squash_texture = LoadTexture("assets/sprites/for/slime_squash.png");
    
    // Set random seed for reproducible behavior
    srand(42);
}

// PERF: ~0.05-0.5ms depending on enemy count
void update_enemies(float dt) {
    for (auto& enemy : slimes) {
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
        enemy.anim_timer += dt;
        if (enemy.anim_timer >= ANIMATION_FRAME_TIME) {
            enemy.anim_timer = 0;
            enemy.anim_frame = (enemy.anim_frame + 1) % enemy.spec->animation_frames;
        }
    }
    
    // Remove dead enemies (move to end and erase)
    slimes.erase(
        std::remove_if(slimes.begin(), slimes.end(), 
            [](const EnemyInstance& enemy) { return !enemy.active || enemy.hp <= 0; }),
        slimes.end()
    );
}

// Demo function to spawn multiple slimes and show behavior
void spawn_demo_slimes(int count) {
    // Clear any existing enemies
    slimes.clear();
    
    TraceLog(LOG_INFO, "spawn_demo_slimes: Spawning %d slimes", count);
    
    // Get player position as center for spawning
    Vector2 player_pos = get_player_position();
    TraceLog(LOG_INFO, "Player position: (%.2f, %.2f)", player_pos.x, player_pos.y);
    
    // Spawn slimes in a circle around the player
    for (int i = 0; i < count; i++) {
        // Calculate position in a circle around the player
        float angle = (360.0f / count) * i * DEG2RAD;
        float distance = 150.0f;  // 150 pixels from the player
        
        Vector2 pos = {
            player_pos.x + cos(angle) * distance,
            player_pos.y + sin(angle) * distance
        };
        
        TraceLog(LOG_INFO, "Spawning slime %d at position: (%.2f, %.2f)", i, pos.x, pos.y);
        spawn_slime(pos);
    }
}

// Toggle visibility of behavior debug info
static bool show_debug_info = false;
void toggle_debug_info() {
    show_debug_info = !show_debug_info;
}

// Enhanced rendering with sprite images
void render_enemies() {
    // Debug output to check if any slimes exist
    static bool logged = false;
    if (!logged) {
        TraceLog(LOG_INFO, "render_enemies: There are %zu slimes in the list", slimes.size());
        logged = true;
    }
    
    for (const auto& enemy : slimes) {
        if (!enemy.active) continue;
        
        TraceLog(LOG_DEBUG, "Rendering slime at world pos: (%.2f, %.2f)", enemy.position.x, enemy.position.y);
        // Convert world position to screen position for rendering
        Vector2 screen_pos = world::world_to_screen(enemy.position);
        
        // Calculate the destination rectangle
        Rectangle dest = {
            screen_pos.x - enemy.spec->size.x/2,
            screen_pos.y - enemy.spec->size.y/2,
            enemy.spec->size.x,
            enemy.spec->size.y
        };
        
        // Choose texture based on animation frame and movement
        Texture2D current_texture = enemy.is_moving && enemy.anim_frame == 1 ? 
            slime_squash_texture : slime_texture;
        
        // Apply tint based on state (attacking or normal)
        Color tint = enemy.attack.attacking ? 
            Color{255, 150, 150, 255} : // Light red tint for attacking
            WHITE;                      // Normal color for idle state
        
        // Draw the slime with proper texture
        DrawTexture(current_texture, 
                   static_cast<int>(dest.x),
                   static_cast<int>(dest.y),
                   tint);
        
        // Draw health indicator above slime
        float health_percent = static_cast<float>(enemy.hp) / static_cast<float>(enemy.spec->hp);
        Rectangle health_bar = {
            dest.x,
            dest.y - 5,
            dest.width * health_percent,
            3
        };
        DrawRectangleRec(health_bar, RED);
        
        // Draw debug info if enabled
        if (show_debug_info) {
            // Show detection radius for chase behavior
            if (std::find(enemy.spec->behaviors.begin(), enemy.spec->behaviors.end(), 
                        BehaviorAtom::chase_player) != enemy.spec->behaviors.end()) {
                DrawCircleLines(
                    screen_pos.x, screen_pos.y, 
                    enemy.chase.detection_radius, 
                    enemy.chase.chasing ? RED : GRAY
                );
            }
            
            // Show attack radius if available
            if (std::find(enemy.spec->behaviors.begin(), enemy.spec->behaviors.end(), 
                        BehaviorAtom::attack_player) != enemy.spec->behaviors.end()) {
                DrawCircleLines(
                    screen_pos.x, screen_pos.y, 
                    enemy.attack.attack_radius, 
                    enemy.attack.can_attack ? ORANGE : DARKGRAY
                );
            }
            
            // Show wander target if active
            if (enemy.wander.has_target) {
                // Convert wander target to screen coordinates
                Vector2 target_screen = world::world_to_screen(enemy.wander.target);
                
                DrawLine(
                    screen_pos.x, screen_pos.y,
                    target_screen.x, target_screen.y,
                    BLUE
                );
                DrawCircle(target_screen.x, target_screen.y, 3, BLUE);
            }
        }
    }
    
    // Show enemy count
    if (show_debug_info) {
        char text[32];
        snprintf(text, sizeof(text), "Enemies: %zu", slimes.size());
        DrawText(text, 10, 10, 20, WHITE);
    }
}

void spawn_slime(Vector2 position) {
    TraceLog(LOG_INFO, "Spawning slime at position: (%.2f, %.2f)", position.x, position.y);
    slimes.emplace_back(SLIME_SPEC, position);
}

bool hit_enemy_at(const Rectangle& hit_rect, const Hit& hit) {
    bool any_hit = false;
    
    for (auto& enemy : slimes) {
        if (!enemy.active) continue;
        
        // Check if hit rectangle overlaps with enemy
        if (CheckCollisionRecs(hit_rect, enemy.collision_rect)) {
            // Apply damage
            enemy.hp -= hit.dmg;
            
            // Apply knockback
            enemy.position.x += hit.knockback.x;
            enemy.position.y += hit.knockback.y;
            
            // Update collision rectangle
            enemy.collision_rect.x = enemy.position.x - enemy.spec->size.x/2;
            enemy.collision_rect.y = enemy.position.y - enemy.spec->size.y/2;
            
            // Check if enemy died
            if (enemy.hp <= 0) {
                // Spawn drops based on chances
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(1, 100);
                
                for (const auto& drop : enemy.spec->drops) {
                    if (dis(gen) <= drop.chance) {
                        // In a full implementation, this would spawn actual pickup items
                        // For now, just log it
                        const char* drop_name = drop.type == DropType::Heart ? "Heart" : "Coin";
                        TraceLog(LOG_INFO, "Enemy dropped: %s", drop_name);
                    }
                }
                
                // Mark as inactive (will be removed in update)
                enemy.active = false;
            }
            
            any_hit = true;
        }
    }
    
    return any_hit;
}

void cleanup_enemies() {
    // Unload textures to prevent memory leaks
    UnloadTexture(slime_texture);
    UnloadTexture(slime_squash_texture);
    
    // Clear slimes list
    slimes.clear();
}

const EnemySpec& get_slime_spec() {
    return SLIME_SPEC;
}

} // namespace enemy 