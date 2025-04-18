/// controller.cpp — implementation of player controller molecule
#include "controller.hpp"
#include "../atoms/debug_draw.hpp"
#include "core/public/world.hpp"
#include "core/public/ui.hpp"

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
    
    // Initialize health using the atom
    health_ = atoms::make_health(10); // 10 pips total
    
    // Tell the world where the player is (for camera)
    core::world::set_camera_target(movement_.position);
    
    // No need to initialize UI hearts here - handled in main.cpp
}

void PlayerController::update(float dt) {
    // Don't process input if not alive
    if (!is_alive()) return;

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
        bool world_collision_x = !core::world::is_position_walkable(test_position_x.x, test_position_x.y);
        
        // Handle X collision
        if (result_x.collided || world_collision_x) {
            // Keep previous X position
            movement_.position.x = previous_position.x;
        }
        
        // Now test Y movement with the resolved X position
        Vector2 test_position_y = { movement_.position.x, movement_.position.y };
        atoms::CollisionResult result_y = collision_world_.test_collision(player_collision_id_, test_position_y);
        
        // Also check against world tilemap
        bool world_collision_y = !core::world::is_position_walkable(test_position_y.x, test_position_y.y);
        
        // Handle Y collision
        if (result_y.collided || world_collision_y) {
            // Keep previous Y position
            movement_.position.y = previous_position.y;
        }
        
        // Final position check (for diagonal collisions)
        atoms::CollisionResult final_result = collision_world_.test_collision(player_collision_id_, movement_.position);
        bool world_final_collision = !core::world::is_position_walkable(movement_.position.x, movement_.position.y);
        
        if (final_result.collided || world_final_collision) {
            // Fallback to previous position completely if still colliding
            movement_.position = previous_position;
        }
        
        // Update collision object position
        collision_world_.update_object_position(player_collision_id_, movement_.position);
        
        // Update camera target position
        core::world::set_camera_target(movement_.position);
    }
    
    // Get texture size for screen bounds checking
    const Texture2D& current_texture = atoms::get_current_frame(animation_);
    
    // Get world bounds
    float min_x, min_y, max_x, max_y;
    core::world::get_bounds(&min_x, &min_y, &max_x, &max_y);
    
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
    
    // Update UI health display
    core::ui::update_health_display(health_.current, health_.max);
    
    // Toggle collision visualization with C key
    if (IsKeyPressed(KEY_C)) {
        show_collision_shapes_ = !show_collision_shapes_;
    }
}

void PlayerController::render() {
    // Get current frame texture
    const Texture2D& texture = atoms::get_current_frame(animation_);
    
    // Convert world position to screen position for drawing
    Vector2 screen_pos = core::world::world_to_screen(movement_.position);
    
    // Draw player sprite centered on position with appropriate color
    Color player_color = is_alive() ? WHITE : GRAY; // Use is_alive() for color
    
    DrawTextureV(texture, 
                Vector2{screen_pos.x - texture.width / 2.0f, 
                        screen_pos.y - texture.height / 2.0f}, 
                player_color);
    
    // Draw collision shapes if enabled
    if (show_collision_shapes_) {
        // Draw our collision objects transformed to screen space
        for (const auto& obj : collision_world_.get_objects()) {
            Vector2 obj_screen_pos = core::world::world_to_screen(obj.position);
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
    
    // Use core interface for debug text
    core::ui::set_debug_text(state_text, 
                            {movement_.position.x - 30, movement_.position.y - 50},
                            WHITE);
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

Vector2 PlayerController::get_position() const {
    return movement_.position;
}

void PlayerController::update_animation_state() {
    // Set animation state based on current actions and movement
    if (!is_alive()) {
        atoms::set_animation_state(animation_, PlayerState::IDLE); // Show idle when dead
        return;
    }
    
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

bool PlayerController::take_damage(int pips, Vector2 knockback_dir) {
    // Apply damage using the health atom
    // This returns false if the player was already dead
    if (!atoms::apply_damage(health_, pips)) {
        return false; // Already dead, no damage applied
    }
    
    // Player was alive, damage was applied
    
    // Calculate knockback force
    Vector2 knockback = {
        knockback_dir.x * 5.0f,
        knockback_dir.y * 5.0f
    };
    
    // Apply knockback directly to position
    movement_.position.x += knockback.x;
    movement_.position.y += knockback.y;
    
    // Log the damage
    TraceLog(LOG_INFO, "Player took %d damage with knockback (%.2f, %.2f)",
             pips, knockback.x, knockback.y);
    TraceLog(LOG_INFO, "Player health reduced to %d/%d", 
             health_.current, health_.max);
    
    // Check if player just died
    if (!is_alive()) {
        TraceLog(LOG_INFO, "Player has died!");
        // Could trigger death animation etc. (handled in update_animation_state)
    }
    
    return true; // Damage was successfully applied
}

int PlayerController::get_health() const {
    return health_.current;
}

bool PlayerController::is_alive() const {
    return atoms::is_alive(health_);
}

bool PlayerController::is_attacking() const {
    return actions_.attacking;
}

Rectangle PlayerController::get_attack_rect() const {
    // Calculate attack rectangle in front of player based on direction and animation
    const Texture2D& texture = atoms::get_current_frame(animation_);
    
    Rectangle attack_rect = {
        movement_.position.x - texture.width/2,
        movement_.position.y - texture.height/2,
        static_cast<float>(texture.width),
        static_cast<float>(texture.height)
    };
    
    // If facing right, extend to the right
    if (IsKeyDown(KEY_RIGHT)) {
        attack_rect.x += texture.width * 0.75f;
        attack_rect.width *= 0.5f;
    }
    // If facing left, extend to the left
    else if (IsKeyDown(KEY_LEFT)) {
        attack_rect.width *= 0.5f;
    }
    // If facing down, extend down
    else if (IsKeyDown(KEY_DOWN)) {
        attack_rect.y += texture.height * 0.75f;
        attack_rect.height *= 0.5f;
    }
    // If facing up, extend up
    else if (IsKeyDown(KEY_UP)) {
        attack_rect.height *= 0.5f;
    }
    
    return attack_rect;
}

int PlayerController::get_max_health() const {
    return health_.max;
}

} // namespace molecules
} // namespace player