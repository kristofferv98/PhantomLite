/// behavior_atoms.cpp — Implementation of common behavior atoms

#include "behavior_atoms.hpp"
#include "../world/world.hpp"
#include "shared/opensimplex2/opensimplex2.h"
#include <algorithm>
#include <cmath>

namespace enemies {
namespace atoms {

// Use OpenSimplex2 for natural noise-based wandering
static opensimplex2::Noise noise(42); // Initialize with fixed seed
static bool noise_initialized = true;

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

// Context Steering Implementation
BehaviorResult apply_context_steering(EnemyRuntime& enemy, float dt) {
    // Find best direction from weights
    int best_ray = 0;
    float best_weight = -999.0f;
    
    for (int i = 0; i < enemy.NUM_RAYS; i++) {
        if (enemy.weights[i] > best_weight) {
            best_weight = enemy.weights[i];
            best_ray = i;
        }
    }
    
    // If all directions are blocked or have negative weights, don't move
    if (best_weight < 0.0f) {
        enemy.is_moving = false;
        return BehaviorResult::Failed;
    }
    
    // Handle movement using the enemy's own method
    enemy.apply_steering_movement(dt);
    
    return BehaviorResult::Running;
}

// Wander implementation
BehaviorResult wander_noise(EnemyRuntime& enemy, float dt) {
    // Update noise sampling position - increase speed for more active wandering
    enemy.wander_noise.noise_offset_x += dt * enemy.wander_noise.sway_speed * 1.5f;
    enemy.wander_noise.noise_offset_y += dt * enemy.wander_noise.sway_speed * 1.2f;
    
    // Sample 2D noise for direction, using more octaves for richer patterns
    float angle_offset = static_cast<float>(noise.noise2(
        enemy.wander_noise.noise_offset_x, 
        enemy.wander_noise.noise_offset_y + 500.0f, // offset for different sampling
        5, 0.65f)) * PI * 2.5f; // Wider range for more varied movement
    
    // Calculate direction vector from noise
    float dir_x = cosf(angle_offset);
    float dir_y = sinf(angle_offset);
    Vector2 wander_dir = { dir_x, dir_y };
    
    // Get distance from spawn point
    Vector2 to_spawn = {
        enemy.wander_noise.spawn_point.x - enemy.position.x,
        enemy.wander_noise.spawn_point.y - enemy.position.y
    };
    float dist_to_spawn = sqrtf(to_spawn.x * to_spawn.x + to_spawn.y * to_spawn.y);
    
    // If too far from spawn point, blend in a return vector with smooth falloff
    if (dist_to_spawn > enemy.wander_noise.radius * 0.6f) { // Start earlier at 60% of radius
        // Normalize to_spawn
        float spawn_length = sqrtf(to_spawn.x * to_spawn.x + to_spawn.y * to_spawn.y);
        if (spawn_length > 0) {
            to_spawn.x /= spawn_length;
            to_spawn.y /= spawn_length;
        }
        
        // Calculate falloff strength that increases more rapidly beyond the radius
        // This creates a "soft boundary" that gets progressively stronger
        float radius_ratio = dist_to_spawn / enemy.wander_noise.radius;
        float blend = 0.0f;
        
        if (radius_ratio > 0.6f) {
            // Start with gentle return force at 60% of radius (earlier than before)
            blend = (radius_ratio - 0.6f) / 0.4f; // 0-1 range from 60% to 100% of radius
            blend = blend * blend * 0.7f; // Cubic for sharper increase with 70% max influence
        }
        
        if (radius_ratio > 1.0f) {
            // Beyond radius, use stronger return force
            blend = 0.7f + (radius_ratio - 1.0f) * 2.0f; // Increase more sharply beyond 100%
            blend = fminf(blend, 0.9f); // Cap at 90% to still allow some noise influence
        }
        
        // Add noise to return direction for more natural path finding
        float noise_influence = 0.2f * (1.0f - blend); // Reduced noise as blend increases
        Vector2 noisy_return = {
            to_spawn.x + static_cast<float>(noise.noise2(enemy.position.x * 0.02f, enemy.position.y * 0.02f, 2, 0.5f)) * noise_influence,
            to_spawn.y + static_cast<float>(noise.noise2(enemy.position.y * 0.02f, enemy.position.x * 0.02f, 2, 0.5f)) * noise_influence
        };
        
        // Normalize noisy return vector
        float noisy_length = sqrtf(noisy_return.x * noisy_return.x + noisy_return.y * noisy_return.y);
        if (noisy_length > 0) {
            noisy_return.x /= noisy_length;
            noisy_return.y /= noisy_length;
        }
        
        // Blend the wandering direction with the noisy return-to-spawn direction
        wander_dir.x = wander_dir.x * (1.0f - blend) + noisy_return.x * blend;
        wander_dir.y = wander_dir.y * (1.0f - blend) + noisy_return.y * blend;
        
        // Renormalize
        float wander_length = sqrtf(wander_dir.x * wander_dir.x + wander_dir.y * wander_dir.y);
        if (wander_length > 0) {
            wander_dir.x /= wander_length;
            wander_dir.y /= wander_length;
        }
    }
    
    // Apply weights to all rays, with highest weight in the wander direction
    for (int i = 0; i < enemy.NUM_RAYS; i++) {
        float ray_angle = i * (2.0f * PI / enemy.NUM_RAYS);
        
        // Calculate angle difference (normalized between -PI and PI)
        float angle_diff = ray_angle - atan2f(wander_dir.y, wander_dir.x);
        while (angle_diff > PI) angle_diff -= 2.0f * PI;
        while (angle_diff < -PI) angle_diff += 2.0f * PI;
        
        // Convert angle difference to a weight (1.0 = perfect match, 0.0 = opposite direction)
        float weight = cosf(angle_diff);
        weight = weight * weight; // Square for sharper falloff from optimal direction
        
        // Apply weight multiplier and scaling - increased from 0.5f to 0.7f for stronger wandering
        weight = weight * 0.7f;
        
        // Add to the ray's weight
        enemy.weights[i] += weight;
    }
    
    return BehaviorResult::Running;
}

// Chase implementation
BehaviorResult chase_direct(EnemyRuntime& enemy, float dt) {
    // Exit if not in chase state
    if (!enemy.chase.chasing) {
        return BehaviorResult::Failed;
    }
    
    // Apply movement toward target
    enemy.apply_steering_movement(dt);
    
    return BehaviorResult::Running;
}

// Attack implementation
BehaviorResult attack_melee(EnemyRuntime& enemy, Vector2 target_pos, float dt) {
    // Access melee attack state
    auto& attack = enemy.attack_melee;
    
    // Check if attack is on cooldown
    if (!attack.can_attack) {
        attack.timer += dt;
        if (attack.timer >= attack.cooldown) {
            attack.timer = 0;
            attack.can_attack = true;
        }
        return BehaviorResult::Failed;
    }
    
    // Calculate distance to target
    float dx = target_pos.x - enemy.position.x;
    float dy = target_pos.y - enemy.position.y;
    float dist = sqrtf(dx * dx + dy * dy);
    
    // If in range, perform attack
    if (dist <= attack.reach) {
        // Start attack
        attack.attacking = true;
        attack.can_attack = false;
        attack.timer = 0;
        attack.attack_timer = 0;
        
        // Face toward target
        if (fabsf(dx) > fabsf(dy)) {
            enemy.facing = (dx > 0) ? Facing::RIGHT : Facing::LEFT;
        } else {
            enemy.facing = (dy > 0) ? Facing::DOWN : Facing::UP;
        }
        
        // Color flash for attack
        enemy.color = RED;
        
        // Log the attack
        TraceLog(LOG_INFO, "Enemy performed melee attack on player");
        
        return BehaviorResult::Running;
    }
    
    return BehaviorResult::Failed;
}

// Strafe implementation
BehaviorResult strafe_around(EnemyRuntime& enemy, Vector2 target_pos, float dt) {
    // Calculate vector to target
    float dx = target_pos.x - enemy.position.x;
    float dy = target_pos.y - enemy.position.y;
    float dist = sqrtf(dx * dx + dy * dy);
    
    // Normalize if not zero
    if (dist > 0) {
        dx /= dist;
        dy /= dist;
    } else {
        return BehaviorResult::Failed;  // Can't strafe if at exact same position
    }
    
    // Apply movement
    enemy.apply_steering_movement(dt);
    
    return BehaviorResult::Running;
}

// Charge dash implementation
BehaviorResult charge_dash(EnemyRuntime& enemy, Vector2 target_pos, float dt) {
    auto& dash = enemy.charge_dash;
    
    // State machine for dash behavior
    switch (dash.state) {
        case ChargeDash::State::Idle: {
            // Start charging
            dash.state = ChargeDash::State::Charging;
            dash.charge_timer = 0;
            
            // Store direction toward target
            float dx = target_pos.x - enemy.position.x;
            float dy = target_pos.y - enemy.position.y;
            float dist = sqrtf(dx * dx + dy * dy);
            
            if (dist > 0) {
                dash.dash_direction.x = dx / dist;
                dash.dash_direction.y = dy / dist;
            } else {
                // Default to right if somehow at exact target position
                dash.dash_direction.x = 1.0f;
                dash.dash_direction.y = 0.0f;
            }
            
            // Visual feedback for charging
            enemy.color = YELLOW;
            
            break;
        }
        
        case ChargeDash::State::Charging: {
            // Update charge timer
            dash.charge_timer += dt;
            
            // Flash color more intensely as charge builds
            float intensity = 0.5f + 0.5f * (dash.charge_timer / dash.charge_duration);
            enemy.color = (Color){
                (unsigned char)(255 * intensity),
                (unsigned char)(255 * 0.5f * intensity),
                0,
                255
            };
            
            // When charged, start dash
            if (dash.charge_timer >= dash.charge_duration) {
                dash.state = ChargeDash::State::Dashing;
                dash.dash_timer = 0;
                
                // Visual feedback for dash
                enemy.color = RED;
            }
            break;
        }
        
        case ChargeDash::State::Dashing: {
            // Update dash timer
            dash.dash_timer += dt;
            
            // Apply movement in dash direction
            float dash_speed = enemy.spec->speed * dash.dash_speed;
            enemy.position.x += dash.dash_direction.x * dash_speed * dt;
            enemy.position.y += dash.dash_direction.y * dash_speed * dt;
            
            // Update facing direction
            if (fabsf(dash.dash_direction.x) > fabsf(dash.dash_direction.y)) {
                enemy.facing = (dash.dash_direction.x > 0) ? Facing::RIGHT : Facing::LEFT;
            } else {
                enemy.facing = (dash.dash_direction.y > 0) ? Facing::DOWN : Facing::UP;
            }
            
            // When dash complete, go to cooldown
            if (dash.dash_timer >= dash.dash_duration) {
                dash.state = ChargeDash::State::Cooldown;
                dash.cooldown_timer = 0;
                
                // Reset color
                enemy.color = WHITE;
            }
            break;
        }
        
        case ChargeDash::State::Cooldown: {
            // Update cooldown timer
            dash.cooldown_timer += dt;
            
            // When cooldown complete, return to idle
            if (dash.cooldown_timer >= dash.cooldown_duration) {
                dash.state = ChargeDash::State::Idle;
                return BehaviorResult::Completed;
            }
            break;
        }
    }
    
    return BehaviorResult::Running;
}

// Seek toward a target position
BehaviorResult seek_target(EnemyRuntime& enemy, Vector2 target, float dt) {
    // Calculate direction to target
    Vector2 dir = direction_to(enemy.position, target);
    float dist = sqrtf(dir.x * dir.x + dir.y * dir.y);
    
    // Normalize direction
    if (dist > 0) {
        dir.x /= dist;
        dir.y /= dist;
    }
    
    // Apply weights based on seek direction
    apply_seek_weights(enemy, target, 1.0f);
    
    // Apply movement
    enemy.apply_steering_movement(dt);
    
    return BehaviorResult::Running;
}

// Strafe/orbit around a target position
BehaviorResult strafe_target(EnemyRuntime& enemy, Vector2 target, float dt) {
    // Calculate direction to target
    Vector2 dir = direction_to(enemy.position, target);
    float dist = sqrtf(dir.x * dir.x + dir.y * dir.y);
    
    // Mark strafe as active for tracking
    enemy.strafe_target.active = true;
    
    // Apply strafe weights
    apply_strafe_weights(enemy, target, enemy.strafe_target.direction, enemy.strafe_target.orbit_gain);
    
    // If too close to target and we have a charge dash behavior, consider using it
    if (static_cast<int>(enemy.spec->behavior_flags & BehaviorFlags::CHARGE_DASH) != 0) {
        if (dist < enemy.strafe_target.orbit_radius * 0.7f && (rand() % 100) < 5) { // 5% chance when close
            // Switch to charge dash
            return charge_dash(enemy, target, dt);
        }
    }
    
    // Periodically change orbit direction to be less predictable
    if ((rand() % 100) < 1) { // 1% chance per frame to switch directions
        enemy.strafe_target.direction *= -1;
    }
    
    return BehaviorResult::Running;
}

// Maintain distance from other enemies
BehaviorResult separate_allies(EnemyRuntime& enemy, const std::vector<EnemyRuntime>& enemies, float dt) {
    // Apply separation weights
    apply_separation_weights(enemy, enemies, enemy.separate_allies.desired_spacing, 1.0f);
    
    // Apply movement
    enemy.apply_steering_movement(dt);
    
    return BehaviorResult::Running;
}

// Avoid obstacles using raycasts
BehaviorResult avoid_obstacles(EnemyRuntime& enemy, float dt) {
    // Apply obstacle avoidance weights
    apply_obstacle_avoidance_weights(enemy, enemy.avoid_obstacle.lookahead_px, 1.0f);
    
    // Apply movement
    enemy.apply_steering_movement(dt);
    
    return BehaviorResult::Running;
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
    
    // Get distance to target
    float dist = distance(enemy.position, target);
    
    // Calculate ideal orbit distance
    float ideal_dist = enemy.strafe_target.orbit_radius;
    
    // Get direction toward or away from target based on current distance
    float dist_gain = 0.0f;
    if (dist > ideal_dist * 1.2f) {
        // Too far, add some inward weight
        dist_gain = 0.5f;
    } else if (dist < ideal_dist * 0.8f) {
        // Too close, add some outward weight
        dist_gain = -0.5f;
    }
    
    // Add noise variation to the strafing direction for more natural movement
    // Makes the enemy occasionally reverse direction or adjust orbit radius
    float time_factor = enemy.wander_noise.noise_offset_x; // Use wander noise time as a convenient source
    
    // Sample noise to occasionally modulate the strafe direction
    float direction_noise = static_cast<float>(noise.noise2(
        enemy.position.x * 0.01f + time_factor * 0.2f,
        enemy.position.y * 0.01f + time_factor * 0.3f,
        2, 0.6f));
    
    // Occasionally flip strafe direction based on noise
    int effective_direction = direction;
    if (direction_noise > 0.8f) { // 20% chance to reverse
        effective_direction = -direction;
    }
    
    // Add noise to orbit distance for less robotic movement
    float orbit_noise = static_cast<float>(noise.noise2(
        enemy.position.x * 0.02f + time_factor * 0.1f,
        enemy.position.y * 0.02f + time_factor * 0.15f,
        2, 0.5f));
    
    // Adjust distance correction based on noise
    dist_gain += (orbit_noise - 0.5f) * 0.3f; // -0.15 to +0.15 adjustment
    
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
        if ((effective_direction > 0 && cross > 0) || (effective_direction < 0 && cross < 0)) {
            // Use sqrt(1-dot²) which approximates the sine of the angle
            // This creates a weight that peaks at perpendicular angles (90°)
            strafe_weight = sqrtf(1.0f - dot * dot);
            
            // Add slight bias toward forward movement for more natural circling
            float forward_bias = 0.2f * fmaxf(0.0f, dot);
            strafe_weight += forward_bias;
        } else {
            // Negative weight for wrong direction
            strafe_weight = -sqrtf(1.0f - dot * dot) * 0.4f; // Reduced negative to allow more flexibility
        }
        
        // Apply strafe weight
        enemy.weights[i] += strafe_weight * gain;
        
        // Apply distance correction if needed
        if (dist_gain != 0.0f) {
            enemy.weights[i] += dot * dist_gain * gain;
        }
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
            // Calculate repulsion strength (stronger as we get closer using inverse square)
            float proximity = (desired_dist - dist) / desired_dist; // 0-1 range
            float repulsion = proximity * proximity * gain; // Squared for stronger effect at close range
            
            // Get direction from other to self (away from other enemy)
            Vector2 repel_dir = normalize(direction_to(other.position, enemy.position));
            
            // Apply repulsion to each ray
            for (int i = 0; i < enemy.NUM_RAYS; i++) {
                Vector2 ray_dir = enemy.get_ray_dir(i);
                float dot = dot_product(repel_dir, ray_dir);
                
                // Apply stronger weights for rays pointing directly away
                float weight = (dot > 0) ? dot * dot * repulsion : 0; // Squared for emphasis on direct paths
                enemy.weights[i] += weight;
            }
        }
    }
}

// Apply weights to avoid obstacles using raycasts
void apply_obstacle_avoidance_weights(EnemyRuntime& enemy, float lookahead_dist, float gain) {
    // Cast rays in all directions
    for (int i = 0; i < enemy.NUM_RAYS; i++) {
        Vector2 ray_dir = enemy.get_ray_dir(i);
        
        // Start and end points for raycast
        Vector2 start = enemy.position;
        Vector2 end = {
            start.x + ray_dir.x * lookahead_dist,
            start.y + ray_dir.y * lookahead_dist
        };
        
        // Check if the ray endpoint is walkable using world collision system
        bool hit_obstacle = !world::is_walkable(end.x, end.y);
        
        if (hit_obstacle) {
            // Raycast to find approximate hit distance
            const int NUM_SAMPLES = 8;
            float hit_distance = lookahead_dist;
            
            // Simple binary search for hit point
            for (int s = 0; s < NUM_SAMPLES; s++) {
                float test_dist = hit_distance * 0.5f;
                Vector2 test_point = {
                    start.x + ray_dir.x * test_dist,
                    start.y + ray_dir.y * test_dist
                };
                
                if (!world::is_walkable(test_point.x, test_point.y)) {
                    hit_distance = test_dist;
                } else {
                    hit_distance = hit_distance * 0.5f + test_dist * 0.5f;
                }
            }
            
            // Stronger negative weight for closer obstacles
            float falloff = 1.0f - (hit_distance / lookahead_dist);
            falloff = falloff * falloff; // Square for sharper falloff
            enemy.weights[i] = -falloff * gain * 2.0f;
        }
    }
}

// Helper for context steering
void apply_steering_movement(EnemyRuntime& enemy, float dt) {
    // Find best direction from weights
    int best_ray = 0;
    float best_weight = -999.0f;
    
    for (int i = 0; i < enemy.NUM_RAYS; i++) {
        if (enemy.weights[i] > best_weight) {
            best_weight = enemy.weights[i];
            best_ray = i;
        }
    }
    
    // If all directions are blocked or have negative weights, don't move
    if (best_weight < 0.0f) {
        enemy.is_moving = false;
        return;
    }
    
    // Get direction vector from the best ray
    float ray_angle = best_ray * (2.0f * PI / enemy.NUM_RAYS);
    Vector2 move_dir = {
        cosf(ray_angle),
        sinf(ray_angle)
    };
    
    // Set the target velocity based on the ray direction and enemy speed
    Vector2 target_velocity = {
        move_dir.x * enemy.spec->speed,
        move_dir.y * enemy.spec->speed
    };
    
    // Smoothly approach the target velocity
    const float acceleration = enemy.spec->speed * 4.0f; // Faster acceleration
    enemy.velocity.x = enemy.approach(enemy.velocity.x, target_velocity.x, acceleration * dt);
    enemy.velocity.y = enemy.approach(enemy.velocity.y, target_velocity.y, acceleration * dt);
    
    // We're moving this frame
    enemy.is_moving = true;
}

// Debug visualization for steering weights
void draw_steering_weights(const EnemyRuntime& enemy, bool screen_space) {
    Vector2 base_pos = enemy.position;
    
    // Convert to screen space if needed
    if (screen_space) {
        base_pos = world::world_to_screen(base_pos);
    }
    
    // Find the strongest ray for emphasis
    int strongest_ray = 0;
    float strongest_weight = -999.0f;
    
    for (int i = 0; i < enemy.NUM_RAYS; i++) {
        if (fabsf(enemy.weights[i]) > fabsf(strongest_weight)) {
            strongest_weight = enemy.weights[i];
            strongest_ray = i;
        }
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
            // Green gradient for positive weights
            unsigned char intensity = (unsigned char)(200.0f * fminf(weight, 1.0f) + 55.0f);
            color = (Color){ 0, intensity, 0, 200 };
        } else if (weight < 0) {
            // Red gradient for negative weights (obstacles)
            unsigned char intensity = (unsigned char)(200.0f * fminf(fabsf(weight), 1.0f) + 55.0f);
            color = (Color){ intensity, 0, 0, 200 };
        } else {
            // Gray for neutral weights
            color = (Color){ 100, 100, 100, 100 };
        }
        
        // Calculate endpoint
        Vector2 end_pos = {
            base_pos.x + ray_dir.x * length,
            base_pos.y + ray_dir.y * length
        };
        
        // Draw the ray with thicker line for the strongest direction
        float line_thickness = (i == strongest_ray) ? 3.0f : 1.5f;
        DrawLineEx(base_pos, end_pos, line_thickness, color);
        
        // Add a small circle at the end of the strongest ray
        if (i == strongest_ray) {
            DrawCircleV(end_pos, 5.0f, color);
        }
    }
    
    // Draw circle at position for reference
    DrawCircleV(base_pos, 5.0f, WHITE);
    
    // Show active behavior states as text
    char behavior_text[128] = "";
    int text_y_offset = -65;
    
    // Show active behaviors
    if (enemy.chase.chasing) {
        strcat(behavior_text, "CHASE ");
    }
    if (enemy.strafe_target.active) {
        strcat(behavior_text, "STRAFE ");
    }
    if (enemy.attack_melee.attacking) {
        strcat(behavior_text, "ATTACK ");
    }
    if (enemy.charge_dash.state != ChargeDash::State::Idle) {
        strcat(behavior_text, "DASH ");
    }
    if (behavior_text[0] == '\0') {
        strcat(behavior_text, "WANDER");
    }
    
    // Draw behavior text
    DrawText(
        behavior_text,
        base_pos.x - MeasureText(behavior_text, 14) / 2,
        base_pos.y + text_y_offset,
        14,
        YELLOW
    );
    
    // Show current health
    DrawText(
        TextFormat("HP: %d/%d", enemy.hp, enemy.spec->hp),
        base_pos.x - 20,
        base_pos.y + text_y_offset - 15,
        12,
        WHITE
    );
}

// Apply weights for a specific direction
void apply_direction_weights(EnemyRuntime& enemy, Vector2 direction, float gain) {
    // Normalize direction
    float len = sqrtf(direction.x * direction.x + direction.y * direction.y);
    if (len > 0) {
        direction.x /= len;
        direction.y /= len;
    } else {
        // Default to upward if no direction provided
        direction.y = -1.0f;
    }
    
    // Apply weights based on alignment with direction
    for (int i = 0; i < enemy.NUM_RAYS; i++) {
        Vector2 ray_dir = enemy.get_ray_dir(i);
        
        // Calculate dot product to determine alignment
        float dot = ray_dir.x * direction.x + ray_dir.y * direction.y;
        
        // Apply weight based on alignment (1.0 for perfect alignment, -1.0 for opposite)
        enemy.weights[i] += dot * gain;
    }
}

} // namespace atoms
} // namespace enemies 