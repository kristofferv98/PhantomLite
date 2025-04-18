/// main.cpp — engine bootstrap & event loop
#include <raylib.h>
#include <fmt/core.h>
#include "features/player/player.hpp"
#include "features/world/world.hpp"
#include "features/ui/ui.hpp"

int main() {
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(1280, 720, "PhantomLite — Zelda-like World Demo");
    SetTargetFPS(60);

    // Initialize game systems and features
    world::init();   // Initialize world first
    player::init();  // Then initialize player 
    ui::init_ui();   // Then initialize UI

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        
        // Update game logic
        world::update(dt);
        player::update(dt);
        ui::update_ui(dt);
        
        // Render frame
        BeginDrawing();
        ClearBackground(BLACK);
        
        // Draw game elements (world first, then player)
        world::render();
        player::render();
        
        // Draw UI on top of everything else
        ui::render_ui();
        
        // Draw FPS counter
        DrawFPS(10, 10);
        
        EndDrawing();
    }
    
    // Cleanup before exit (in reverse order)
    ui::cleanup_ui();
    player::cleanup();
    world::cleanup();
    CloseWindow();
    return 0;
} 