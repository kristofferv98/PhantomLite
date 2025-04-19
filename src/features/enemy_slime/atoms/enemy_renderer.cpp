/// enemy_renderer.cpp — Enemy rendering atom implementation

#include "enemy_renderer.hpp"
#include "enemy_state.hpp"
#include "../../world/world.hpp"
#include "features/enemies/behavior_atoms.hpp" // Include for steering visualization
#include <raylib.h>
#include <raymath.h>
#include <cmath>

namespace enemy {
namespace atoms {

// Texture for slime sprites
static Texture2D slime_texture;
static Texture2D slime_squash_texture;

// Debug visualization flag
static bool show_debug = false;
static bool show_steering_debug = false; // Flag for steering visualization

void init_renderer() {
    // Load enemy textures
    slime_texture = LoadTexture("assets/sprites/slime.png");
    slime_squash_texture = LoadTexture("assets/sprites/slime_squash.png");
    
    // Debug disabled by default
    show_debug = false;
    show_steering_debug = false;
}

// Draw the ray context steering for debugging
void draw_steering_rays(const enemies::EnemyRuntime& enemy) {
    // Get position in screen coordinates
    Vector2 screen_pos = world::world_to_screen(enemy.position);
    
    // Find the strongest ray (most influencing direction)
    float strongest_weight = 0;
    int strongest_dir = -1;
    
    for (int i = 0; i < enemy.NUM_RAYS; i++) {
        if (fabsf(enemy.weights[i]) > fabsf(strongest_weight)) {
            strongest_weight = enemy.weights[i];
            strongest_dir = i;
        }
    }
    
    // Draw each ray with color indicating its weight
    for (int i = 0; i < enemy.NUM_RAYS; i++) {
        float angle = i * (2.0f * PI / enemy.NUM_RAYS);
        float cos_angle = cosf(angle);
        float sin_angle = sinf(angle);
        
        // Determine color based on weight (green for positive, red for negative)
        Color ray_color;
        float weight_abs = fabsf(enemy.weights[i]);
        float weight_norm = fminf(weight_abs, 1.0f); // Normalize weight for visualization
        
        if (enemy.weights[i] > 0) {
            // Positive weight - green with alpha based on strength
            ray_color = ColorAlpha(GREEN, weight_norm);
        } else if (enemy.weights[i] < 0) {
            // Negative weight - red with alpha based on strength
            ray_color = ColorAlpha(RED, weight_norm);
        } else {
            // Zero weight - grey
            ray_color = ColorAlpha(GRAY, 0.3f);
        }
        
        // Determine ray length based on weight
        float ray_length = 50.0f * fmaxf(weight_norm, 0.2f);
        
        // Create end point
        Vector2 end_pos = {
            screen_pos.x + cos_angle * ray_length,
            screen_pos.y + sin_angle * ray_length
        };
        
        // Draw the ray
        DrawLineEx(screen_pos, end_pos, i == strongest_dir ? 3.0f : 1.0f, ray_color);
        
        // If this is the strongest ray, add a circle at the end
        if (i == strongest_dir) {
            DrawCircleV(end_pos, 5.0f, ray_color);
        }
    }
    
    // Draw a label showing current behaviors in effect
    const enemies::EnemyRuntime& e = enemy;
    
    // Build behavior string
    char behavior_text[128] = "";
    bool has_behaviors = false;
    
    if (e.attack_melee.attacking) {
        strcat(behavior_text, "ATTACK ");
        has_behaviors = true;
    }
    if (e.chase.chasing) {
        strcat(behavior_text, "CHASE ");
        has_behaviors = true;
    }
    if (e.strafe_target.active) {
        strcat(behavior_text, "STRAFE ");
        has_behaviors = true;
    }
    if (!has_behaviors) {
        strcat(behavior_text, "WANDER");
    }
    
    // Draw the behavior text above the enemy
    DrawText(
        behavior_text,
        screen_pos.x - MeasureText(behavior_text, 14) / 2,
        screen_pos.y - 60,
        14,
        YELLOW
    );
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
        if (enemy.anim_frame == 1 || enemy.attack_melee.attacking) {
            texture = &slime_squash_texture;
        }
        
        // Draw the enemy
        // Center the texture on the enemy position
        DrawTextureV(*texture, 
                   Vector2{screen_pos.x - texture->width/2, screen_pos.y - texture->height/2}, 
                   enemy.color);
        
        // Draw debug visualization if enabled
        if (show_debug) {
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
            if (enemy.attack_melee.attacking) {
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
            
            // Draw velocity vector
            if (enemy.is_moving) {
                Vector2 vel_end = {
                    screen_pos.x + enemy.velocity.x * 0.5f, // Scale for visibility
                    screen_pos.y + enemy.velocity.y * 0.5f
                };
                DrawLineEx(screen_pos, vel_end, 2.0f, GREEN);
                DrawCircleV(vel_end, 3.0f, GREEN);
            }
            
            // Draw knockback vector if active
            if (enemy.knockback.x != 0 || enemy.knockback.y != 0) {
                Vector2 kb_end = {
                    screen_pos.x + enemy.knockback.x * 0.1f, // Scale for visibility
                    screen_pos.y + enemy.knockback.y * 0.1f
                };
                DrawLineEx(screen_pos, kb_end, 2.0f, RED);
                DrawCircleV(kb_end, 3.0f, RED);
            }
            
            // Draw steering visualization if enabled
            if (show_steering_debug) {
                draw_steering_rays(enemy);
            }
        }
    }
    
    // Draw global debug info
    if (show_debug) {
        // Draw number of enemies
        DrawText(TextFormat("Enemies: %d", enemies.size()), 10, 40, 20, WHITE);
        
        // Draw steering debug status
        const char* steering_text = show_steering_debug ? "Steering: ON" : "Steering: OFF";
        DrawText(steering_text, 10, 70, 20, show_steering_debug ? GREEN : GRAY);
    }
}

void set_debug_visualization(bool enabled) {
    show_debug = enabled;
    
    // Log the state change
    if (enabled) {
        TraceLog(LOG_INFO, "Enemy debug visualization enabled");
    } else {
        TraceLog(LOG_INFO, "Enemy debug visualization disabled");
        // Also disable steering debug when main debug is disabled
        show_steering_debug = false;
    }
}

void toggle_debug_visualization() {
    set_debug_visualization(!show_debug);
}

// New function to toggle steering visualization
void toggle_steering_debug() {
    // Only toggle if main debug is enabled
    if (show_debug) {
        show_steering_debug = !show_steering_debug;
        TraceLog(LOG_INFO, "Steering debug visualization %s", show_steering_debug ? "enabled" : "disabled");
    }
}

bool is_debug_visualization_enabled() {
    return show_debug;
}

bool is_steering_debug_enabled() {
    return show_steering_debug;
}

void cleanup_renderer() {
    // Unload textures
    UnloadTexture(slime_texture);
    UnloadTexture(slime_squash_texture);
}

} // namespace atoms
} // namespace enemy 