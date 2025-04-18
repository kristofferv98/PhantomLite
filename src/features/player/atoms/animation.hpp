/// animation.hpp — animation atom for player slice
#pragma once
#include <raylib.h>
#include <unordered_map>
#include "../player.hpp" // For PlayerState enum

namespace player {
namespace atoms {

struct AnimationFrame {
    Texture2D texture;
};

struct AnimationClip {
    AnimationFrame* frames;
    int frame_count;
    int current_frame;
    float frame_time;
    float timer;
};

struct AnimationSystem {
    std::unordered_map<PlayerState, AnimationClip> clips;
    PlayerState current_state;
};

// Load animation frames for a specific state
void load_animation(AnimationSystem& system, PlayerState state, const char* filepaths[], int frame_count);

// Update animation frames based on elapsed time
void update_animation(AnimationSystem& system, float dt);

// Set the current animation state
void set_animation_state(AnimationSystem& system, PlayerState state);

// Get the current frame texture to render
const Texture2D& get_current_frame(const AnimationSystem& system);

// Cleanup animation resources
void cleanup_animations(AnimationSystem& system);

// Create a new animation system
AnimationSystem create_animation_system();

} // namespace atoms
} // namespace player 