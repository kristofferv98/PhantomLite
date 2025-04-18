/// actions.cpp — implementation of actions atom
#include "actions.hpp"

namespace player {
namespace atoms {

ActionState create_action_state() {
    return {
        .attacking = false,
        .attack_timer = 0.0f,
        .attack_duration = 0.4f  // Default attack animation duration
    };
}

bool process_actions(ActionState& state, float dt) {
    bool state_changed = false;
    
    // If already attacking, update timer
    if (state.attacking) {
        state.attack_timer += dt;
        
        // Check if attack is complete
        if (state.attack_timer >= state.attack_duration) {
            state.attacking = false;
            state.attack_timer = 0.0f;
            state_changed = true;
        }
    }
    // Otherwise, check for attack input
    else if (IsKeyPressed(KEY_SPACE)) {
        state.attacking = true;
        state.attack_timer = 0.0f;
        state_changed = true;
    }
    
    return state_changed;
}

bool is_attack_complete(const ActionState& state) {
    return state.attacking && state.attack_timer >= state.attack_duration;
}

} // namespace atoms
} // namespace player 