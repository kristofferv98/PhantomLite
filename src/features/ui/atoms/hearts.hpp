/// hearts.hpp — Heart UI atom for player and enemy health display

#pragma once

#include <vector>
#include "raylib.h"

namespace ui {

// Forward declaration
class CameraView;

/// Manages heart display for health (e.g., 12 pips = 3 hearts)
class Hearts {
public:
    Hearts(int max_pips = 12, int current_pips = 12);
    
    /// Take damage, reducing current_pips (clamped to 0)
    void take_damage(int pips);
    
    /// Heal, increasing current_pips (clamped to max_pips)
    void heal(int pips);
    
    /// Draw hearts UI at fixed screen positions
    void draw_hearts(const Rectangle& screen_area);
    
    /// Get current health as percentage (0.0f to 1.0f)
    float get_health_percent() const;
    
    /// Get current pips
    int get_current_pips() const { return current_pips; }
    
    /// Get max pips
    int get_max_pips() const { return max_pips; }

private:
    int current_pips;  // Current health in pips
    int max_pips;      // Maximum health in pips
    Texture2D heart_full;  // Full heart texture
    Texture2D heart_empty; // Empty heart texture
    
    /// Initialize textures
    void load_textures();
    
    /// Unload textures to prevent memory leaks
    void unload_textures();
};

} // namespace ui 