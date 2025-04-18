/// behavior_atoms.cpp — Implementation of common behavior atoms

#include "behavior_atoms.hpp"
#include "../world/world.hpp"
#include <algorithm>
#include <cmath>

namespace enemies {
namespace atoms {

// Utility functions for vector operations
namespace {
    // Get vector from a to b
    Vector2 direction_to(const Vector2& a, const Vector2& b) {
        return { b.x - a.x, b.y - a.y };
    }
    
    // Get distance between points
    float distance(const Vector2& a, const Vector2& b) {
        float dx = b.x - a.x;
        float dy = b.y - a.y;
        return sqrtf(dx * dx + dy * dy);
    }
    
    // Normalize a vector
    Vector2 normalize(const Vector2& v) {
        float len = sqrtf(v.x * v.x + v.y * v.y);
        if (len > 0) {
            return { v.x / len, v.y / len };
        }
        return { 0, 0 };
    }
    
    // Calculate dot product
    float dot_product(const Vector2& a, const Vector2& b) {
        return a.x * b.x + a.y * b.y;
    }
    
    // Calculate 2D cross product (for direction)
    float cross_product(const Vector2& a, const Vector2& b) {
        return a.x * b.y - a.y * b.x;
    }
    
    // Simple 2D Perlin noise approximation
    // This is a very simplified version - in a real game, use a proper noise library
    float simple_noise(float x, float y) {
        // We're just using this to get some variation - not real Perlin noise
        float n = sinf(x) * 0.5f + cosf(y) * 0.5f;
        return n * 0.5f + 0.5f; // Scale to 0-1 range
    }
}

// Wander using noise for smooth paths
BehaviorResult wander_noise(EnemyRuntime& enemy, float dt) {
    auto& wander = enemy.wander_noise;
    
    // Update noise offsets
    wander.noise_offset_x += wander.sway_speed * dt;
    wander.noise_offset_y += wander.sway_speed * dt * 1.3f; // Different rate for variety
    
    // Reset weights array
    enemy.reset_weights();
    
    // Calculate wander target based on noise
    float noise_val_x = simple_noise(wander.noise_offset_x, 0.0f) * 2.0f - 1.0f; // -1 to 1
    float noise_val_y = simple_noise(0.0f, wander.noise_offset_y) * 2.0f - 1.0f; // -1 to 1
    
    // Generate target position from noise
    Vector2 wander_target = {
        wander.spawn_point.x + noise_val_x * wander.radius,
        wander.spawn_point.y + noise_val_y * wander.radius
    };
    
    // Apply seeking behavior toward the wander target
    apply_seek_weights(enemy, wander_target, 0.8f); // Lower gain for gentler wandering
    
    // Apply the steering movement
    enemy.apply_steering_movement(enemy.spec->speed, dt);
    
    return BehaviorResult::Running; // Wandering is ongoing
}

// Seek toward a target position
BehaviorResult seek_target(EnemyRuntime& enemy, Vector2 target, float dt) {
    auto& seek = enemy.seek_target;
    float dist = distance(enemy.position, target);
    
    // If we've reached our preferred distance, we're done
    if (fabsf(dist - seek.preferred_dist) < 5.0f) {
        seek.active = false;
        return BehaviorResult::Completed;
    }
    
    // Still need to seek
    seek.active = true;
    
    // Reset weights
    enemy.reset_weights();
    
    // Apply seeking behavior
    apply_seek_weights(enemy, target, seek.seek_gain);
    
    // Apply movement
    enemy.apply_steering_movement(enemy.spec->speed, dt);
    
    return BehaviorResult::Running;
}

// Strafe/orbit around a target position
BehaviorResult strafe_target(EnemyRuntime& enemy, Vector2 target, float dt) {
    auto& strafe = enemy.strafe_target;
    float dist = distance(enemy.position, target);
    
    // Set active flag
    strafe.active = true;
    
    // Reset weights
    enemy.reset_weights();
    
    // First, maintain proper orbit distance
    float orbit_diff = dist - strafe.orbit_radius;
    if (fabsf(orbit_diff) > 10.0f) {
        // Need to move closer/further to maintain orbit radius
        float seek_gain = orbit_diff > 0 ? -0.5f : 0.5f; // Move away or toward
        apply_seek_weights(enemy, target, seek_gain);
    }
    
    // Apply strafing weights
    apply_strafe_weights(enemy, target, strafe.direction, strafe.orbit_gain);
    
    // Apply movement
    enemy.apply_steering_movement(enemy.spec->speed, dt);
    
    return BehaviorResult::Running; // Strafing is ongoing
}

// Maintain distance from other enemies
BehaviorResult separate_allies(EnemyRuntime& enemy, const std::vector<EnemyRuntime>& enemies, float dt) {
    auto& separate = enemy.separate_allies;
    
    // Reset weights
    enemy.reset_weights();
    
    // Apply separation weights
    apply_separation_weights(enemy, enemies, separate.desired_spacing, separate.separation_gain);
    
    // Apply movement
    enemy.apply_steering_movement(enemy.spec->speed, dt);
    
    return BehaviorResult::Running; // Separation is ongoing
}

// Avoid obstacles using raycasts
BehaviorResult avoid_obstacles(EnemyRuntime& enemy, float dt) {
    auto& avoid = enemy.avoid_obstacle;
    
    // We don't reset weights here, so this can be combined with other behaviors
    
    // Apply obstacle avoidance weights
    apply_obstacle_avoidance_weights(enemy, avoid.lookahead_px, avoid.avoidance_gain);
    
    // Apply movement
    enemy.apply_steering_movement(enemy.spec->speed, dt);
    
    return BehaviorResult::Running; // Avoidance is ongoing
}

// Apply a charge and dash attack
BehaviorResult charge_dash(EnemyRuntime& enemy, Vector2 target, float dt) {
    auto& dash = enemy.charge_dash;
    
    // State machine for charge-dash behavior
    switch (dash.state) {
        case ChargeDash::State::Idle:
            // Begin charging
            dash.state = ChargeDash::State::Charging;
            dash.charge_timer = 0.0f;
            // Set dash direction toward target
            dash.dash_direction = normalize(direction_to(enemy.position, target));
            return BehaviorResult::Running;
            
        case ChargeDash::State::Charging:
            // Update charge timer
            dash.charge_timer += dt;
            if (dash.charge_timer >= dash.charge_duration) {
                // Done charging, start dash
                dash.state = ChargeDash::State::Dashing;
                dash.dash_timer = 0.0f;
            }
            return BehaviorResult::Running;
            
        case ChargeDash::State::Dashing:
            // Update dash timer
            dash.dash_timer += dt;
            
            // Apply dash movement
            enemy.position.x += dash.dash_direction.x * enemy.spec->speed * dash.dash_speed * dt;
            enemy.position.y += dash.dash_direction.y * enemy.spec->speed * dash.dash_speed * dt;
            
            // Update collision rectangle
            enemy.collision_rect.x = enemy.position.x - enemy.spec->size.x/2;
            enemy.collision_rect.y = enemy.position.y - enemy.spec->size.y/2;
            
            // Set facing direction based on dash direction
            if (fabsf(dash.dash_direction.x) > fabsf(dash.dash_direction.y)) {
                enemy.facing = dash.dash_direction.x > 0 ? Facing::RIGHT : Facing::LEFT;
            } else {
                enemy.facing = dash.dash_direction.y > 0 ? Facing::DOWN : Facing::UP;
            }
            
            // Check if dash is complete
            if (dash.dash_timer >= dash.dash_duration) {
                // Done dashing, enter cooldown
                dash.state = ChargeDash::State::Cooldown;
                dash.cooldown_timer = 0.0f;
            }
            
            // We're moving
            enemy.is_moving = true;
            return BehaviorResult::Running;
            
        case ChargeDash::State::Cooldown:
            // Update cooldown timer
            dash.cooldown_timer += dt;
            if (dash.cooldown_timer >= dash.cooldown_duration) {
                // Cooldown complete, reset to idle
                dash.state = ChargeDash::State::Idle;
                return BehaviorResult::Completed;
            }
            return BehaviorResult::Running;
    }
    
    return BehaviorResult::Failed; // Should never get here
}

// Apply a ranged attack
BehaviorResult ranged_shoot(EnemyRuntime& enemy, Vector2 target, float dt) {
    auto& shoot = enemy.ranged_shoot;
    
    // Update timer
    if (!shoot.can_fire) {
        shoot.timer += dt;
        if (shoot.timer >= shoot.cooldown) {
            shoot.can_fire = true;
            shoot.timer = 0.0f;
        }
    }
    
    // Check if we can fire
    if (shoot.can_fire) {
        // Calculate direction to target
        Vector2 fire_dir = normalize(direction_to(enemy.position, target));
        
        // Spawn projectile
        // TODO: Actual projectile creation - this is just a stub
        TraceLog(LOG_INFO, "Enemy fired projectile at player");
        
        // Start cooldown
        shoot.can_fire = false;
        shoot.timer = 0.0f;
        
        return BehaviorResult::Completed;
    }
    
    return BehaviorResult::Failed; // Can't fire yet
}

// Apply a melee attack
BehaviorResult attack_melee(EnemyRuntime& enemy, Vector2 target, float dt) {
    auto& melee = enemy.attack_melee;
    
    // Update timer if on cooldown
    if (!melee.can_attack) {
        melee.timer += dt;
        if (melee.timer >= melee.cooldown) {
            melee.can_attack = true;
            melee.timer = 0.0f;
        }
    }
    
    // Update attack animation if attacking
    if (melee.attacking) {
        melee.attack_timer += dt;
        if (melee.attack_timer >= melee.attack_duration) {
            melee.attacking = false;
            return BehaviorResult::Completed;
        }
        return BehaviorResult::Running;
    }
    
    // Check if we can attack and if target is in range
    float dist = distance(enemy.position, target);
    if (melee.can_attack && dist <= melee.reach) {
        // Start attack
        melee.attacking = true;
        melee.can_attack = false;
        melee.attack_timer = 0.0f;
        
        // Calculate direction to target and set facing
        Vector2 attack_dir = normalize(direction_to(enemy.position, target));
        if (fabsf(attack_dir.x) > fabsf(attack_dir.y)) {
            enemy.facing = attack_dir.x > 0 ? Facing::RIGHT : Facing::LEFT;
        } else {
            enemy.facing = attack_dir.y > 0 ? Facing::DOWN : Facing::UP;
        }
        
        // TODO: Call function to deal damage to target
        TraceLog(LOG_INFO, "Enemy performed melee attack on player");
        
        return BehaviorResult::Running;
    }
    
    return BehaviorResult::Failed; // Can't attack yet or target not in range
}

// Process the steering weights and apply movement
BehaviorResult apply_context_steering(EnemyRuntime& enemy, float dt) {
    // Find best direction from weights
    int best_ray = 0;
    float best_weight = -999.0f;
    
    for (int i = 0; i < enemy.NUM_RAYS; i++) {
        if (enemy.weights[i] > best_weight && enemy.weights[i] >= 0) {
            best_weight = enemy.weights[i];
            best_ray = i;
        }
    }
    
    // If all directions are blocked or have negative weights, don't move
    if (best_weight < 0.0f) {
        enemy.is_moving = false;
        return BehaviorResult::Failed;
    }
    
    // Get direction vector from the best ray
    Vector2 move_dir = enemy.get_ray_dir(best_ray);
    
    // Apply movement
    float adjusted_speed = enemy.spec->speed * dt;
    enemy.position.x += move_dir.x * adjusted_speed;
    enemy.position.y += move_dir.y * adjusted_speed;
    
    // Update collision rectangle
    enemy.collision_rect.x = enemy.position.x - enemy.spec->size.x/2;
    enemy.collision_rect.y = enemy.position.y - enemy.spec->size.y/2;
    
    // Update facing direction based on movement
    if (fabsf(move_dir.x) > fabsf(move_dir.y)) {
        enemy.facing = move_dir.x > 0 ? Facing::RIGHT : Facing::LEFT;
    } else {
        enemy.facing = move_dir.y > 0 ? Facing::DOWN : Facing::UP;
    }
    
    // We moved this frame
    enemy.is_moving = true;
    
    return BehaviorResult::Running;
}

// Helper functions for weight calculations

// Apply weights for seeking a target
void apply_seek_weights(EnemyRuntime& enemy, Vector2 target, float gain) {
    // Get direction to target
    Vector2 target_dir = normalize(direction_to(enemy.position, target));
    
    // Calculate weights for each ray
    for (int i = 0; i < enemy.NUM_RAYS; i++) {
        // Get ray direction
        Vector2 ray_dir = enemy.get_ray_dir(i);
        
        // Calculate how closely this ray points toward target (dot product)
        float dot = dot_product(target_dir, ray_dir);
        
        // Apply weight based on alignment with target direction
        enemy.weights[i] += dot * gain;
    }
}

// Apply weights for strafing around a target
void apply_strafe_weights(EnemyRuntime& enemy, Vector2 target, int direction, float gain) {
    // Get direction to target
    Vector2 target_dir = normalize(direction_to(enemy.position, target));
    
    // Calculate weights for each ray
    for (int i = 0; i < enemy.NUM_RAYS; i++) {
        // Get ray direction
        Vector2 ray_dir = enemy.get_ray_dir(i);
        
        // Calculate dot product (alignment with target direction)
        float dot = dot_product(target_dir, ray_dir);
        
        // Calculate cross product (perpendicular component)
        float cross = cross_product(target_dir, ray_dir);
        
        // Use cross to determine strafing direction
        // cross > 0 means clockwise, < 0 means counterclockwise
        float strafe_weight = 0.0f;
        if ((direction > 0 && cross > 0) || (direction < 0 && cross < 0)) {
            // Use sqrt(1-dot²) which approximates the sine of the angle
            strafe_weight = sqrtf(1.0f - dot * dot);
        } else {
            // Negative weight for wrong direction
            strafe_weight = -sqrtf(1.0f - dot * dot);
        }
        
        // Apply strafe weight
        enemy.weights[i] += strafe_weight * gain;
    }
}

// Apply weights for separation from other entities
void apply_separation_weights(EnemyRuntime& enemy, const std::vector<EnemyRuntime>& enemies, float desired_dist, float gain) {
    // Check each enemy
    for (const auto& other : enemies) {
        // Skip self or dead enemies
        if (&other == &enemy || !other.is_alive()) continue;
        
        // Calculate distance
        float dist = distance(enemy.position, other.position);
        
        // Only consider enemies within desired distance
        if (dist < desired_dist) {
            // Calculate repulsion strength (stronger as we get closer)
            float repulsion = (1.0f - dist / desired_dist) * gain;
            
            // Get direction from other to self
            Vector2 repel_dir = normalize(direction_to(other.position, enemy.position));
            
            // Apply repulsion to each ray
            for (int i = 0; i < enemy.NUM_RAYS; i++) {
                Vector2 ray_dir = enemy.get_ray_dir(i);
                float dot = dot_product(repel_dir, ray_dir);
                
                // Only apply positive weights in the direction away from the other entity
                if (dot > 0) {
                    enemy.weights[i] += dot * repulsion;
                }
            }
        }
    }
}

// Apply weights to avoid obstacles using raycasts
void apply_obstacle_avoidance_weights(EnemyRuntime& enemy, float lookahead_dist, float gain) {
    // Cast rays in all directions
    for (int i = 0; i < enemy.NUM_RAYS; i++) {
        Vector2 ray_dir = enemy.get_ray_dir(i);
        
        // Cast ray to check for obstacles
        // This is a simplified version - in a real game, use proper raycasting
        bool hit_obstacle = false;
        
        // Start and end points for raycast
        Vector2 start = enemy.position;
        Vector2 end = {
            start.x + ray_dir.x * lookahead_dist,
            start.y + ray_dir.y * lookahead_dist
        };
        
        // Check if the ray endpoint is walkable
        hit_obstacle = !world::is_walkable(end.x, end.y);
        
        // If hit obstacle, set negative weight for this direction
        if (hit_obstacle) {
            enemy.weights[i] = -1.0f * gain;
        }
    }
}

// Debug visualization for steering weights
void draw_steering_weights(const EnemyRuntime& enemy, bool screen_space) {
    Vector2 base_pos = enemy.position;
    
    // Convert to screen space if needed
    if (screen_space) {
        base_pos = world::world_to_screen(base_pos);
    }
    
    // Draw a ray for each weight
    for (int i = 0; i < enemy.NUM_RAYS; i++) {
        // Get ray direction
        Vector2 ray_dir = enemy.get_ray_dir(i);
        
        // Calculate weight-scaled length
        float weight = enemy.weights[i];
        float length = fabsf(weight) * 50.0f; // Scale for visibility
        
        // Calculate color based on weight
        Color color;
        if (weight > 0) {
            // Green for positive weights
            color = GREEN;
        } else if (weight < 0) {
            // Red for negative weights (obstacles)
            color = RED;
        } else {
            // Gray for neutral weights
            color = GRAY;
        }
        
        // Adjust alpha for visibility
        color.a = 180;
        
        // Calculate endpoint
        Vector2 end_pos = {
            base_pos.x + ray_dir.x * length,
            base_pos.y + ray_dir.y * length
        };
        
        // Draw the ray
        DrawLineEx(base_pos, end_pos, 2.0f, color);
    }
    
    // Draw circle at position for reference
    DrawCircleV(base_pos, 5.0f, WHITE);
}

} // namespace atoms
} // namespace enemies 