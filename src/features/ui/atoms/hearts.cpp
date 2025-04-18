/// hearts.cpp — health display and management implementation

#include "hearts.hpp"
#include <algorithm>

namespace ui {

Hearts::Hearts(int initial_max_pips) 
    : current_pips(initial_max_pips), max_pips(initial_max_pips) {
}

void Hearts::take_damage(int pips) {
    current_pips = std::max(0, current_pips - pips);
}

void Hearts::heal(int pips) {
    current_pips = std::min(max_pips, current_pips + pips);
}

void Hearts::draw_hearts(const Rectangle& viewport) const {
    // Position hearts at the top-left of the screen with some padding
    const int padding_x = 10;
    const int padding_y = 10;
    
    int hearts_total = max_pips / PIPS_PER_HEART;
    int hearts_full = current_pips / PIPS_PER_HEART;
    int pips_remainder = current_pips % PIPS_PER_HEART;
    
    // Draw full hearts
    for (int i = 0; i < hearts_full; i++) {
        Rectangle heart_pos = {
            viewport.x + padding_x + i * HEART_SPACING,
            viewport.y + padding_y,
            static_cast<float>(HEART_SIZE),
            static_cast<float>(HEART_SIZE)
        };
        
        // In a real implementation, we'd use a sprite but for now use DrawRectangle
        DrawRectangle(heart_pos.x, heart_pos.y, heart_pos.width, heart_pos.height, RED);
    }
    
    // Draw partial heart if needed
    if (pips_remainder > 0 && hearts_full < hearts_total) {
        Rectangle heart_pos = {
            viewport.x + padding_x + hearts_full * HEART_SPACING,
            viewport.y + padding_y,
            static_cast<float>(HEART_SIZE),
            static_cast<float>(HEART_SIZE)
        };
        
        // Draw partial heart based on remaining pips
        float fraction = static_cast<float>(pips_remainder) / PIPS_PER_HEART;
        DrawRectangle(heart_pos.x, heart_pos.y, heart_pos.width * fraction, heart_pos.height, RED);
        
        // Draw empty part
        DrawRectangleLines(heart_pos.x, heart_pos.y, heart_pos.width, heart_pos.height, RED);
    }
    
    // Draw empty hearts
    for (int i = hearts_full + (pips_remainder > 0 ? 1 : 0); i < hearts_total; i++) {
        Rectangle heart_pos = {
            viewport.x + padding_x + i * HEART_SPACING,
            viewport.y + padding_y,
            static_cast<float>(HEART_SIZE),
            static_cast<float>(HEART_SIZE)
        };
        
        // Draw empty heart outline
        DrawRectangleLines(heart_pos.x, heart_pos.y, heart_pos.width, heart_pos.height, RED);
    }
}

} // namespace ui 