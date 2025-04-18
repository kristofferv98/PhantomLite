/// hearts.cpp — Heart UI atom implementation

#include "hearts.hpp"
#include <algorithm>
#include "raylib.h"

namespace ui {

// Number of health pips per heart
static const int PIPS_PER_HEART = 4;

// PERF: Negligible, only called once at startup and draws <10 sprites per frame
Hearts::Hearts(int max_pips, int current_pips)
    : current_pips(current_pips), max_pips(max_pips) {
    load_textures();
}

void Hearts::load_textures() {
    // Placeholder textures, in production these would be loaded from assets/
    // Create placeholder heart textures - in production this would use proper sprites
    Image heart_full_img = GenImageColor(16, 16, RED);
    Image heart_empty_img = GenImageColor(16, 16, DARKGRAY);
    
    heart_full = LoadTextureFromImage(heart_full_img);
    heart_empty = LoadTextureFromImage(heart_empty_img);
    
    UnloadImage(heart_full_img);
    UnloadImage(heart_empty_img);
}

void Hearts::unload_textures() {
    UnloadTexture(heart_full);
    UnloadTexture(heart_empty);
}

void Hearts::take_damage(int pips) {
    current_pips = std::max(0, current_pips - pips);
}

void Hearts::heal(int pips) {
    current_pips = std::min(max_pips, current_pips + pips);
}

float Hearts::get_health_percent() const {
    return static_cast<float>(current_pips) / static_cast<float>(max_pips);
}

void Hearts::draw_hearts(const Rectangle& screen_area) {
    // Calculate the total number of hearts to display
    // (Use ceiling division to make sure we display enough hearts)
    int hearts_total = (max_pips + PIPS_PER_HEART - 1) / PIPS_PER_HEART;
    
    // Calculate how many full hearts to display
    int hearts_full = current_pips / PIPS_PER_HEART;
    
    // Calculate the remainder pips (partial heart)
    int pips_remainder = current_pips % PIPS_PER_HEART;
    
    const int hearts_per_row = 5;
    const int heart_size = 16;
    const int heart_padding = 2;
    
    // Calculate starting position (top-left corner)
    float start_x = screen_area.x + 10; // Padding from left edge
    float start_y = screen_area.y + 10; // Padding from top edge
    
    for (int i = 0; i < hearts_total; i++) {
        // Calculate position for this heart
        float x = start_x + (i % hearts_per_row) * (heart_size + heart_padding);
        float y = start_y + (i / hearts_per_row) * (heart_size + heart_padding);
        
        // Draw full or empty heart
        if (i < hearts_full) {
            // Draw full heart
            DrawTexture(heart_full, x, y, WHITE);
        } else if (i == hearts_full && pips_remainder > 0) {
            // Draw partial heart (simplified to just show full for now)
            // In a real implementation, you would use different textures for different fill levels
            DrawTexture(heart_full, x, y, WHITE);
        } else {
            // Draw empty heart
            DrawTexture(heart_empty, x, y, WHITE);
        }
    }
}

} // namespace ui 