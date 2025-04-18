#include "hearts_controller.hpp"
#include <iostream>
#include <cmath>

namespace player {

std::vector<HeartState> HeartsController::heart_states;
Texture2D HeartsController::heart_full;
Texture2D HeartsController::heart_three_quarter;
Texture2D HeartsController::heart_half;
Texture2D HeartsController::heart_quarter;
Texture2D HeartsController::heart_empty;
bool HeartsController::initialized = false;

void HeartsController::init() {
    if (initialized) return;
    
    // Load heart textures
    heart_full = LoadTexture("assets/ui/heart_full.png");
    heart_three_quarter = LoadTexture("assets/ui/heart_three_quarter.png");
    heart_half = LoadTexture("assets/ui/heart_half.png");
    heart_quarter = LoadTexture("assets/ui/heart_quarter.png");
    
    // Create an empty heart texture or use the quarter one with transparency
    heart_empty = LoadTexture("assets/ui/heart_empty.png");
    
    if (heart_full.id == 0 || heart_three_quarter.id == 0 || 
        heart_half.id == 0 || heart_quarter.id == 0) {
        std::cerr << "Error loading heart textures!" << std::endl;
    }
    
    initialized = true;
}

void HeartsController::cleanup() {
    if (!initialized) return;
    
    UnloadTexture(heart_full);
    UnloadTexture(heart_three_quarter);
    UnloadTexture(heart_half);
    UnloadTexture(heart_quarter);
    UnloadTexture(heart_empty);
    
    initialized = false;
}

void HeartsController::update(float current_health, float max_health) {
    // Calculate how many hearts we need
    int max_hearts = static_cast<int>(std::ceil(max_health / 4.0f));
    
    // Resize heart states vector if needed
    heart_states.resize(max_hearts, HeartState::Empty);
    
    // Calculate current heart fill states
    float remaining_health = current_health;
    
    for (int i = 0; i < max_hearts; i++) {
        if (remaining_health >= 4.0f) {
            heart_states[i] = HeartState::Full;
            remaining_health -= 4.0f;
        } else if (remaining_health >= 3.0f) {
            heart_states[i] = HeartState::ThreeQuarter;
            remaining_health -= 3.0f;
        } else if (remaining_health >= 2.0f) {
            heart_states[i] = HeartState::Half;
            remaining_health -= 2.0f;
        } else if (remaining_health >= 1.0f) {
            heart_states[i] = HeartState::Quarter;
            remaining_health -= 1.0f;
        } else {
            heart_states[i] = HeartState::Empty;
        }
    }
}

void HeartsController::render(Vector2 position) {
    if (!initialized) return;
    
    // Draw each heart based on its state
    for (size_t i = 0; i < heart_states.size(); i++) {
        Vector2 heart_pos = {
            position.x + i * HEART_SPACING,
            position.y
        };
        
        // Select the appropriate texture based on heart state
        Texture2D* texture = &heart_empty;
        
        switch (heart_states[i]) {
            case HeartState::Full:
                texture = &heart_full;
                break;
            case HeartState::ThreeQuarter:
                texture = &heart_three_quarter;
                break;
            case HeartState::Half:
                texture = &heart_half;
                break;
            case HeartState::Quarter:
                texture = &heart_quarter;
                break;
            case HeartState::Empty:
                texture = &heart_empty;
                break;
        }
        
        // Draw the heart
        DrawTexture(*texture, heart_pos.x, heart_pos.y, WHITE);
    }
}

} // namespace player 