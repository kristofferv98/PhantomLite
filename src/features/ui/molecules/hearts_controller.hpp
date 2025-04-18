/// hearts_controller.hpp — Manages hearts UI and damage processing

#pragma once
#include "../atoms/hearts.hpp"

namespace ui {

class HeartsController {
public:
    // Initialize with default health
    static void init();
    
    // Update the hearts display
    static void update(float dt);
    
    // Draw the hearts to the screen
    static void render();
    
    // Cleanup resources
    static void cleanup();
    
    // Player takes damage
    static void take_damage(int pips);
    
    // Player heals
    static void heal(int pips);
    
    // Get current health status
    static bool is_player_alive();
    
private:
    static Hearts player_hearts;
};

} // namespace ui 