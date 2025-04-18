/// main.cpp — engine bootstrap & event loop
#include <raylib.h>
#include <fmt/core.h>
#include "public/core.hpp" // Main core interface
#include "features/player/player.hpp"
#include "features/world/world.hpp"
#include "features/ui/ui.hpp"
#include "features/enemy_slime/enemy_slime.hpp"
#include "features/player/molecules/hearts_controller.hpp"

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
    
    // Initialize the hearts controller
    player::HeartsController::init();
    
    // Initialize UI health display with player's max health
    ui::update_ui(0.0f); // Initial UI update
    
    // Spawn multiple slimes for demo mode
    enemy::spawn_demo_slimes(5);

    // Debug flags
    bool show_debug = false;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        
        // Update world first (background)
        world::update(dt);
        
        // Update player (only if alive)
        if (player::is_alive()) {
             player::update(dt);
        }
        
        // Update enemies after player
        enemy::update_enemies(dt);
        
        // Update UI last
        ui::update_ui(dt);
        
        // Update hearts based on player health using core interface
        core::ui::update_health_display(
            player::get_health(),
            player::get_max_health()
        );
        
        // Check if player is alive (for game logic like game over)
        if (!player::is_alive()) {
            // Player is dead, could show game over screen or other logic
            // Input is already disabled in player::update
        }
        
        // Handle player attack input (only if alive)
        if (player::is_alive() && IsKeyPressed(KEY_SPACE)) {
            // Get player attack rectangle
            Rectangle attack_rect = player::get_attack_rect();
            
            // Create a hit
            enemy::Hit hit = {
                .dmg = 1,
                .knockback = {10.0f, 0.0f}, // TODO: Make knockback based on player facing
                .type = enemy::Hit::Type::Melee
            };
            
            // Check for enemy hits
            if (enemy::hit_enemy_at(attack_rect, hit)) {
                TraceLog(LOG_INFO, "Player hit an enemy!");
            }
        }
        
        // Toggle debug info visualization with D key
        if (IsKeyPressed(KEY_D)) {
            enemy::toggle_debug_info();
            show_debug = !show_debug;
        }
        
        // Spawn more slimes with S key
        if (IsKeyPressed(KEY_S)) {
            // Get world dimensions
            float min_x, min_y, max_x, max_y;
            core::world::get_bounds(&min_x, &min_y, &max_x, &max_y);
            
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
        
        // Draw hearts in the top right corner
        player::HeartsController::render({ 
            static_cast<float>(GetScreenWidth() - 150), 
            10.0f 
        });
        
        // Debug info
        if (show_debug) {
            DrawFPS(10, 10);
            
            // Show player health and position for debugging
            Vector2 player_pos = core::entity::get_player_position();
            core::ui::set_debug_text(
                TextFormat("Player Pos: (%.0f, %.0f) Health: %d/%d", 
                            player_pos.x, player_pos.y, 
                            player::get_health(), player::get_max_health()),
                {10, 40},
                WHITE
            );
        }
        
        // Draw controls help - split between left and right sides
        DrawText("Arrows: Move", 
                 10, GetScreenHeight() - 30, 20, RAYWHITE);
        DrawText("SPACE: Attack", 
                 GetScreenWidth() - 200, GetScreenHeight() - 30, 20, RAYWHITE);
        DrawText("D: Toggle Debug Info, S: Spawn Slime", 
                 10, GetScreenHeight() - 55, 20, RAYWHITE);
        DrawText("C: Toggle Collision", 
                 GetScreenWidth() - 200, GetScreenHeight() - 55, 20, RAYWHITE);
        
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