/// main.cpp — engine bootstrap & event loop
#include <raylib.h>
#include <fmt/core.h>
#include "features/player/player.hpp"
#include "features/world/world.hpp"
#include "features/ui/ui.hpp"
#include "features/enemy_slime/enemy_slime.hpp"

int main() {
    SetTraceLogLevel(LOG_INFO);
    InitWindow(1280, 720, "PhantomLite");
    SetTargetFPS(60);

    // Initialize game systems and features
    world::init();     // Initialize world first
    player::init(
        static_cast<float>(GetScreenWidth()) / 2,
        static_cast<float>(GetScreenHeight()) / 2
    );    // Then initialize player 
    ui::init_ui();     // Initialize UI systems
    enemy::init_enemies(); // Initialize enemy systems
    
    // Spawn multiple slimes for demo mode
    enemy::spawn_demo_slimes(5);

    // Debug flags
    bool show_debug = false;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        
        // Update world first (background)
        world::update(dt);
        
        // Update player
        player::update(dt);
        
        // Update enemies after player
        enemy::update_enemies(dt);
        
        // Update UI last
        ui::update_ui(dt);
        
        // Check if player is dead
        if (player::get_health() <= 0) {
            // Player is dead, could show game over screen
            // For now, just continue to show the dead player
        }
        
        // Check for collisions between player weapon (attack) and enemies
        // This would normally be integrated into a proper collision system
        if (IsKeyPressed(KEY_SPACE)) {
            // Simulate a player attack with a hit rect in front of the player
            Vector2 player_pos = player::get_position();
            Rectangle hit_rect = {player_pos.x - 16, player_pos.y - 16, 32, 32};
            
            // Create a hit
            enemy::Hit hit = {
                .dmg = 1,
                .knockback = {10.0f, 0.0f},
                .type = enemy::Hit::Type::Melee
            };
            
            // Check for enemy hits
            if (enemy::hit_enemy_at(hit_rect, hit)) {
                TraceLog(LOG_INFO, "Player hit an enemy!");
            }
        }
        
        // Toggle debug info visualization with D key
        if (IsKeyPressed(KEY_D)) {
            enemy::toggle_debug_info();
        }
        
        // Spawn more slimes with S key
        if (IsKeyPressed(KEY_S)) {
            // Get world dimensions
            float min_x, min_y, max_x, max_y;
            world::get_world_bounds(&min_x, &min_y, &max_x, &max_y);
            
            // Generate position in world coordinates
            float margin = 100.0f;
            float x = min_x + margin + static_cast<float>(GetRandomValue(0, static_cast<int>(max_x - min_x - 2*margin)));
            float y = min_y + margin + static_cast<float>(GetRandomValue(0, static_cast<int>(max_y - min_y - 2*margin)));
            
            enemy::spawn_slime({x, y});
        }
        
        // Render frame
        BeginDrawing();
        ClearBackground(BLACK);
        
        // Render world first (background)
        world::render();
        
        // Render player
        player::render();
        
        // Render enemies
        enemy::render_enemies();
        
        // Render UI on top
        ui::render_ui();
        
        // Debug info
        if (show_debug) {
            DrawFPS(10, 10);
        }
        
        // Draw controls help
        DrawText("Controls: WASD = Move, SPACE = Attack", 
                 GetScreenWidth() - 350, 10, 10, WHITE);
        DrawText("D = Toggle Debug Info, S = Spawn Slime", 
                 GetScreenWidth() - 350, 25, 10, WHITE);
        
        // Show player position for debugging
        Vector2 player_pos = player::get_position();
        char position_text[64];
        sprintf(position_text, "Player Pos: (%.0f, %.0f)", player_pos.x, player_pos.y);
        DrawText(position_text, 10, 30, 10, WHITE);
        
        EndDrawing();
    }
    
    // Cleanup before exit (in reverse order)
    enemy::cleanup_enemies();
    ui::cleanup_ui();
    player::cleanup();
    world::cleanup();
    CloseWindow();
    return 0;
} 