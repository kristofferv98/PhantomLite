/// hearts.hpp — health display and management atom

#pragma once
#include <raylib.h>

namespace ui {

class Hearts {
public:
    Hearts(int initial_max_pips = 12);
    
    void take_damage(int pips);
    void heal(int pips);
    
    void draw_hearts(const Rectangle& viewport) const;
    
    int get_current_pips() const { return current_pips; }
    int get_max_pips() const { return max_pips; }
    bool is_alive() const { return current_pips > 0; }
    
private:
    int current_pips;
    int max_pips;
    
    // Each heart = 4 pips
    static constexpr int PIPS_PER_HEART = 4;
    static constexpr int HEART_SPACING = 36; // pixels between hearts
    static constexpr int HEART_SIZE = 32;    // heart sprite size
};

} // namespace ui 