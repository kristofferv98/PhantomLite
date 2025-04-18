/// actions.hpp — actions atom for player slice (attack, jump, etc.)
#pragma once
#include <raylib.h>

namespace player {
namespace atoms {

struct ActionState {
    bool attacking;
    float attack_timer;
    float attack_duration;
};

// Initialize default action state
ActionState create_action_state();

// Process player action inputs
bool process_actions(ActionState& state, float dt);

// Check if attack is completed
bool is_attack_complete(const ActionState& state);

} // namespace atoms
} // namespace player 