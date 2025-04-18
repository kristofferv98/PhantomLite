#pragma once

#include <vector>
#include <memory>
#include "raylib.h"

namespace player {

enum class HeartState {
    Full,
    ThreeQuarter,
    Half,
    Quarter,
    Empty
};

class HeartsController {
public:
    static void init();
    static void cleanup();
    
    // Update heart display based on current health
    static void update(float current_health, float max_health);
    
    // Render hearts at the specified position (top-left corner)
    static void render(Vector2 position);

private:
    static std::vector<HeartState> heart_states;
    static Texture2D heart_full;
    static Texture2D heart_three_quarter;
    static Texture2D heart_half;
    static Texture2D heart_quarter;
    static Texture2D heart_empty;
    
    static bool initialized;
    static constexpr float HEART_SPACING = 36.0f; // Spacing between hearts
};

} // namespace player 