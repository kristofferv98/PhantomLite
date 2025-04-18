/// controller.hpp — player controller molecule that composes atoms
#pragma once
#include "../atoms/movement.hpp"
#include "../atoms/animation.hpp"
#include "../atoms/actions.hpp"
#include "../atoms/collision.hpp"

namespace player {
namespace molecules {

class PlayerController {
public:
    // Initialize the player controller
    void init(float start_x, float start_y);
    
    // Update player state based on input and time
    void update(float dt);
    
    // Render the player
    void render();
    
    // Clean up resources
    void cleanup();
    
    // Get access to the collision world for adding obstacles
    atoms::CollisionWorld& get_collision_world();
    
    // Get player collision ID
    int get_player_collision_id() const;
    
private:
    // Component states from atoms
    atoms::MovementState movement_;
    atoms::AnimationSystem animation_;
    atoms::ActionState actions_;
    atoms::CollisionWorld collision_world_;
    int player_collision_id_ = -1;
    
    // Debug flags
    bool show_collision_shapes_ = true;
    
    // Update animation state based on movement and actions
    void update_animation_state();
    
    // Create test obstacles (temporary)
    void create_test_obstacles();
};

} // namespace molecules
} // namespace player 