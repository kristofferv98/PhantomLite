/// hearts_controller.hpp — Header for the Hearts UI controller
#pragma once

namespace ui {

/// HeartsController: Static methods to control the player's heart display
class HeartsController {
public:
    /// Initialize the Hearts UI with max health
    static void init(int max_pips);

    /// Take damage, reducing current health
    static void take_damage(int pips);
    
    /// Heal the player
    static void heal(int pips);
    
    /// Check if player is alive
    static bool is_player_alive();
};

} // namespace ui 