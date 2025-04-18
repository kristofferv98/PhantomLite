/// animation.cpp — implementation of animation atom
#include "animation.hpp"

namespace player {
namespace atoms {

AnimationSystem create_animation_system() {
    AnimationSystem system;
    system.current_state = PlayerState::IDLE;
    return system;
}

void load_animation(AnimationSystem& system, PlayerState state, const char* filepaths[], int frame_count) {
    // Allocate memory for frames
    AnimationFrame* frames = new AnimationFrame[frame_count];
    
    // Load all textures
    for (int i = 0; i < frame_count; i++) {
        frames[i].texture = LoadTexture(filepaths[i]);
        if (frames[i].texture.id == 0) {
            TraceLog(LOG_WARNING, "Failed to load animation texture: %s", filepaths[i]);
        }
    }
    
    // Create and store animation clip
    AnimationClip clip = {
        .frames = frames,
        .frame_count = frame_count,
        .current_frame = 0,
        .frame_time = 0.15f,  // Default frame time (configurable)
        .timer = 0.0f
    };
    
    system.clips[state] = clip;
}

void update_animation(AnimationSystem& system, float dt) {
    // Get current animation clip
    auto it = system.clips.find(system.current_state);
    if (it == system.clips.end()) return;
    
    AnimationClip& clip = it->second;
    
    // Update timer and advance frame if needed
    clip.timer += dt;
    if (clip.timer >= clip.frame_time) {
        clip.timer = 0.0f;
        clip.current_frame = (clip.current_frame + 1) % clip.frame_count;
    }
}

void set_animation_state(AnimationSystem& system, PlayerState state) {
    // Don't change state if it's the same
    if (system.current_state == state) return;
    
    // Check if the requested state exists
    auto it = system.clips.find(state);
    if (it == system.clips.end()) {
        TraceLog(LOG_WARNING, "Tried to set animation to non-existent state: %d", static_cast<int>(state));
        return;
    }
    
    // Set new state and reset frame
    system.current_state = state;
    system.clips[state].current_frame = 0;
    system.clips[state].timer = 0.0f;
}

const Texture2D& get_current_frame(const AnimationSystem& system) {
    // Get current animation clip
    auto it = system.clips.find(system.current_state);
    if (it == system.clips.end()) {
        // Return a placeholder texture if state not found
        static Texture2D placeholder = {};
        return placeholder;
    }
    
    const AnimationClip& clip = it->second;
    return clip.frames[clip.current_frame].texture;
}

void cleanup_animations(AnimationSystem& system) {
    // Unload all textures and free memory
    for (auto& pair : system.clips) {
        AnimationClip& clip = pair.second;
        for (int i = 0; i < clip.frame_count; i++) {
            UnloadTexture(clip.frames[i].texture);
        }
        delete[] clip.frames;
    }
    
    system.clips.clear();
}

} // namespace atoms
} // namespace player 