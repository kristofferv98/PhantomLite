/// controller.cpp — implementation of player controller molecule
#include "controller.hpp"
#include "../atoms/debug_draw.hpp"
#include "../../world/world.hpp" // Include world API

namespace player {
namespace molecules {

void PlayerController::init(float start_x, float start_y) {
    // Initialize movement state
    movement_ = atoms::create_movement_state(start_x, start_y, 200.0f);
    
    // Initialize animation system
    animation_ = atoms::create_animation_system();
    
    // Load animations for each state
    const char* idle_frames[] = {"assets/sprites/knight.png"};
    atoms::load_animation(animation_, PlayerState::IDLE, idle_frames, 1);
    
    const char* walking_frames[] = {"assets/sprites/knight.png", "assets/sprites/knight_walk.png"};
    atoms::load_animation(animation_, PlayerState::WALKING, walking_frames, 2);
    
    const char* attack_frames[] = {"assets/sprites/knight_attack.png"};
    atoms::load_animation(animation_, PlayerState::ATTACKING, attack_frames, 1);
    
    // Initialize action state
    actions_ = atoms::create_action_state();
    
    // Initialize collision
    const Texture2D& texture = atoms::get_current_frame(animation_);
    atoms::CollisionShape player_shape = atoms::CollisionShape::Rectangle(
        texture.width * 0.7f,  // Make hitbox slightly smaller than sprite
        texture.height * 0.7f,
        {0, 0}  // Centered on player position
    );
    
    atoms::CollisionObject player_obj = {
        movement_.position,
        player_shape,
        true,  // Solid
        0      // ID will be assigned by the collision world
    };
    
    player_collision_id_ = collision_world_.add_object(player_obj);
    
    // Tell the world where the player is (for camera)
    world::set_camera_target(movement_.position);
}

void PlayerController::update(float dt) {
    // Save previous movement state for comparison
    bool was_moving = movement_.is_moving;
    Vector2 previous_position = movement_.position;
    
    // Process actions first (they have priority)
    bool action_changed = atoms::process_actions(actions_, dt);
    
    // Only process movement if not attacking
    if (!actions_.attacking) {
        // Process input (this updates movement_.position based on input)
        atoms::process_movement(movement_, dt);
        
        // Handle X and Y movement separately to allow sliding along walls
        Vector2 test_position_x = { movement_.position.x, previous_position.y };
        
        // Check X against our own collision objects
        atoms::CollisionResult result_x = collision_world_.test_collision(player_collision_id_, test_position_x);
        
        // Also check against world tilemap
        bool world_collision_x = !world::is_walkable(test_position_x.x, test_position_x.y);
        
        // Handle X collision
        if (result_x.collided || world_collision_x) {
            // Keep previous X position
            movement_.position.x = previous_position.x;
        }
        
        // Now test Y movement with the resolved X position
        Vector2 test_position_y = { movement_.position.x, movement_.position.y };
        atoms::CollisionResult result_y = collision_world_.test_collision(player_collision_id_, test_position_y);
        
        // Also check against world tilemap
        bool world_collision_y = !world::is_walkable(test_position_y.x, test_position_y.y);
        
        // Handle Y collision
        if (result_y.collided || world_collision_y) {
            // Keep previous Y position
            movement_.position.y = previous_position.y;
        }
        
        // Final position check (for diagonal collisions)
        atoms::CollisionResult final_result = collision_world_.test_collision(player_collision_id_, movement_.position);
        bool world_final_collision = !world::is_walkable(movement_.position.x, movement_.position.y);
        
        if (final_result.collided || world_final_collision) {
            // Fallback to previous position completely if still colliding
            movement_.position = previous_position;
        }
        
        // Update collision object position
        collision_world_.update_object_position(player_collision_id_, movement_.position);
        
        // Update camera target position
        world::set_camera_target(movement_.position);
    }
    
    // Get texture size for screen bounds checking
    const Texture2D& current_texture = atoms::get_current_frame(animation_);
    
    // Get world bounds
    float min_x, min_y, max_x, max_y;
    world::get_world_bounds(&min_x, &min_y, &max_x, &max_y);
    
    // Constrain to world bounds
    float half_width = current_texture.width / 2.0f;
    float half_height = current_texture.height / 2.0f;
    movement_.position.x = std::clamp(movement_.position.x, min_x + half_width, max_x - half_width);
    movement_.position.y = std::clamp(movement_.position.y, min_y + half_height, max_y - half_height);
    
    // Update animation system
    atoms::update_animation(animation_, dt);
    
    // Update animation state if action changed OR movement state changed
    if (action_changed || was_moving != movement_.is_moving) {
        update_animation_state();
    }
    
    // Toggle collision visualization with C key
    if (IsKeyPressed(KEY_C)) {
        show_collision_shapes_ = !show_collision_shapes_;
    }
}

void PlayerController::render() {
    // Get current frame texture
    const Texture2D& texture = atoms::get_current_frame(animation_);
    
    // Convert world position to screen position for drawing
    Vector2 screen_pos = world::world_to_screen(movement_.position);
    
    // Draw player sprite centered on position
    DrawTextureV(texture, 
                Vector2{screen_pos.x - texture.width / 2.0f, 
                        screen_pos.y - texture.height / 2.0f}, 
                WHITE);
    
    // Draw collision shapes if enabled
    if (show_collision_shapes_) {
        // Draw our collision objects transformed to screen space
        for (const auto& obj : collision_world_.get_objects()) {
            Vector2 obj_screen_pos = world::world_to_screen(obj.position);
            atoms::draw_collision_shape(obj.shape, obj_screen_pos, RED);
        }
    }
    
    // Debug state display
    const char* state_text = "";
    switch (animation_.current_state) {
        case PlayerState::IDLE: state_text = "IDLE"; break;
        case PlayerState::WALKING: state_text = "WALKING"; break;
        case PlayerState::ATTACKING: state_text = "ATTACKING"; break;
    }
    
    Vector2 text_screen_pos = world::world_to_screen({movement_.position.x - 30, movement_.position.y - 50});
    DrawText(state_text, static_cast<int>(text_screen_pos.x), 
             static_cast<int>(text_screen_pos.y), 20, WHITE);
    
    // Draw controls help
    DrawText("Arrows: Move   Space: Attack   C: Toggle Collision", 
             10, GetScreenHeight() - 30, 20, RAYWHITE);
}

void PlayerController::cleanup() {
    // Clean up animation resources
    atoms::cleanup_animations(animation_);
}

atoms::CollisionWorld& PlayerController::get_collision_world() {
    return collision_world_;
}

int PlayerController::get_player_collision_id() const {
    return player_collision_id_;
}

void PlayerController::update_animation_state() {
    // Set animation state based on current actions and movement
    if (actions_.attacking) {
        atoms::set_animation_state(animation_, PlayerState::ATTACKING);
    } else if (movement_.is_moving) {
        atoms::set_animation_state(animation_, PlayerState::WALKING);
    } else {
        atoms::set_animation_state(animation_, PlayerState::IDLE);
    }
}

void PlayerController::create_test_obstacles() {
    // Create a few test obstacles to demonstrate collision
    
    // Wall on left side of screen
    atoms::CollisionShape wall_shape = atoms::CollisionShape::Rectangle(50, 400);
    atoms::CollisionObject wall_obj = {
        {100, 360},  // Position
        wall_shape,
        true,        // Solid
        0            // ID will be assigned
    };
    collision_world_.add_object(wall_obj);
    
    // Wall on right side of screen
    wall_obj.position.x = GetScreenWidth() - 100;
    collision_world_.add_object(wall_obj);
    
    // Circular obstacle in middle
    atoms::CollisionShape circle_shape = atoms::CollisionShape::Circle(60);
    atoms::CollisionObject circle_obj = {
        {static_cast<float>(GetScreenWidth()) / 2, static_cast<float>(GetScreenHeight()) / 2 + 100},
        circle_shape,
        true,
        0
    };
    collision_world_.add_object(circle_obj);
    
    // Small rectangle obstacle
    atoms::CollisionShape rect_shape = atoms::CollisionShape::Rectangle(80, 80);
    atoms::CollisionObject rect_obj = {
        {static_cast<float>(GetScreenWidth()) / 2 - 200, static_cast<float>(GetScreenHeight()) / 2},
        rect_shape,
        true,
        0
    };
    collision_world_.add_object(rect_obj);
}

} // namespace molecules
} // namespace player 