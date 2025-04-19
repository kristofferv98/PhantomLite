/// enemy_renderer.cpp — Enemy rendering atom implementation

#include "enemy_renderer.hpp"
#include "enemy_state.hpp"
#include "behavior_atoms.hpp"  // Include to access debug flag functions
#include "../../world/world.hpp"
#include "features/enemies/behavior_atoms.hpp" // Include for steering visualization
#include <raylib.h>

namespace enemy {
namespace atoms {

// Texture for slime sprites
static Texture2D slime_texture;
static Texture2D slime_squash_texture;

void init_renderer() {
    // Load enemy textures
    slime_texture = LoadTexture("assets/sprites/slime.png");
    slime_squash_texture = LoadTexture("assets/sprites/slime_squash.png");
}

// PERF: ~0.2-1.0ms depending on enemy count and screen size
void render_enemies() {
    const std::vector<enemies::EnemyRuntime>& enemies = get_enemies();
    
    // Log for debugging
    TraceLog(LOG_INFO, "render_enemies: There are %d slimes in the list", enemies.size());
    
    // Render each enemy
    for (const auto& enemy : enemies) {
        if (!enemy.active) continue;
        
        // Get enemy position in screen coordinates using world->screen conversion
        Vector2 screen_pos = world::world_to_screen(enemy.position);
        
        // Use different texture based on animation frame
        Texture2D* texture = &slime_texture;
        if (enemy.anim_frame == 1 || enemy.attack.attacking) {
            texture = &slime_squash_texture;
        }
        
        // Draw the enemy
        // Center the texture on the enemy position
        DrawTextureV(*texture, 
                   Vector2{screen_pos.x - texture->width/2, screen_pos.y - texture->height/2}, 
                   enemy.color);
        
        // Draw debug visualization if enabled
        if (is_debug_visualization_enabled()) {
            // Draw collision rectangle
            Rectangle collision_rect_screen = {
                world::world_to_screen(Vector2{enemy.collision_rect.x, enemy.collision_rect.y}).x,
                world::world_to_screen(Vector2{enemy.collision_rect.x, enemy.collision_rect.y}).y,
                enemy.collision_rect.width,  // No scaling needed, world_to_screen handles it
                enemy.collision_rect.height  // No scaling needed, world_to_screen handles it
            };
            DrawRectangleLinesEx(collision_rect_screen, 1.0f, RED);
            
            // Draw detection radius
            float detection_radius = enemy.spec->detection_radius; 
            DrawCircleLines(screen_pos.x, screen_pos.y, detection_radius, BLUE);
            
            // Draw attack radius
            float attack_radius = enemy.spec->attack_radius;
            DrawCircleLines(screen_pos.x, screen_pos.y, attack_radius, RED);
            
            // Draw enemy health
            DrawText(TextFormat("HP: %d/%d", enemy.hp, enemy.spec->hp), 
                    screen_pos.x - 20, 
                    screen_pos.y - enemy.spec->size.y - 10, 
                    10, WHITE);
            
            // Draw state information
            const char* state;
            if (enemy.attack.attacking) {
                state = "ATTACK";
            } else if (enemy.chase.chasing) {
                state = "CHASE";
            } else if (enemy.is_moving) {
                state = "MOVE";
            } else {
                state = "IDLE";
            }
            
            DrawText(state, 
                    screen_pos.x - 20, 
                    screen_pos.y - enemy.spec->size.y - 25, 
                    10, YELLOW);
            
            // Draw enemy type name
            DrawText(enemy.spec->name.c_str(),
                    screen_pos.x - 20,
                    screen_pos.y - enemy.spec->size.y - 40,
                    10, GREEN);
            
            // Draw steering visualization if enabled
            if (is_steering_debug_enabled()) {
                enemies::atoms::draw_steering_weights(enemy, true); // true = screen space
            }
        }
    }
    
    // Draw global debug info
    if (is_debug_visualization_enabled()) {
        // Draw number of enemies
        DrawText(TextFormat("Enemies: %d", enemies.size()), 10, 40, 20, WHITE);
        
        // Draw steering debug status
        const char* steering_text = is_steering_debug_enabled() ? "Steering: ON" : "Steering: OFF";
        DrawText(steering_text, 10, 70, 20, is_steering_debug_enabled() ? GREEN : GRAY);
    }
}

void cleanup_renderer() {
    // Unload textures
    UnloadTexture(slime_texture);
    UnloadTexture(slime_squash_texture);
}

} // namespace atoms
} // namespace enemy 

