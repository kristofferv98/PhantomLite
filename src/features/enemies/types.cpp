/// types.cpp — Implementation of enemy types and runtime state

#include "types.hpp"
#include "../world/world.hpp"
#include <cmath>
#include <algorithm>

namespace enemies {

// EnemyRuntime implementation
void EnemyRuntime::on_hit(const Hit& hit) {
    // Don't apply damage if already at 0 HP
    if (hp <= 0) return;
    
    // Apply damage
    hp -= hit.dmg;
    
    // Flash red to indicate damage
    color = RED; // Set color to red temporarily
    
    // Apply knockback
    if (hp > 0 && (hit.knockback.x != 0 || hit.knockback.y != 0)) {
        // Use the knockback vector from the hit
        knockback = hit.knockback;
        
        // Apply knockback based on enemy's resistance
        knockback.x *= (1.0f - knockback_resistance);
        knockback.y *= (1.0f - knockback_resistance);
        
        // Set knockback recovery timer
        knockback_timer = knockback_duration;
    }
}

float EnemyRuntime::approach(float current, float target, float amount) {
    if (current < target) {
        return std::min(current + amount, target);
    } else {
        return std::max(current - amount, target);
    }
}

void EnemyRuntime::apply_steering_movement(float dt) {
    // Find best direction from weights
    int best_ray = 0;
    float best_weight = -999.0f;
    
    for (int i = 0; i < NUM_RAYS; i++) {
        if (weights[i] > best_weight) {
            best_weight = weights[i];
            best_ray = i;
        }
    }
    
    // If all directions are blocked or have negative weights, don't move
    if (best_weight < 0.0f) {
        is_moving = false;
        return;
    }
    
    // Get direction vector from the best ray
    float ray_angle = best_ray * (2.0f * PI / NUM_RAYS);
    Vector2 move_dir = {
        cosf(ray_angle),
        sinf(ray_angle)
    };
    
    // Set the target velocity based on the ray direction
    Vector2 target_velocity = {
        move_dir.x * spec->speed,
        move_dir.y * spec->speed
    };
    
    // Smoothly approach the target velocity
    const float acceleration = spec->speed * 4.0f; // Faster acceleration
    velocity.x = approach(velocity.x, target_velocity.x, acceleration * dt);
    velocity.y = approach(velocity.y, target_velocity.y, acceleration * dt);
    
    // Apply knockback (if active)
    if (knockback_timer > 0) {
        knockback_timer -= dt;
        
        // Reduce knockback over time
        float knockback_factor = knockback_timer / knockback_duration;
        position.x += knockback.x * knockback_factor * dt;
        position.y += knockback.y * knockback_factor * dt;
        
        // If knockback is still strong, reduce regular movement
        if (knockback_factor > 0.5f) {
            velocity.x *= 0.5f;
            velocity.y *= 0.5f;
        }
    }
    
    // Apply velocity to position
    position.x += velocity.x * dt;
    position.y += velocity.y * dt;
    
    // Update collision rectangle
    collision_rect.x = position.x - spec->size.x/2;
    collision_rect.y = position.y - spec->size.y/2;
    
    // Update facing direction based on movement
    if (fabsf(velocity.x) > fabsf(velocity.y) && fabsf(velocity.x) > 5.0f) {
        facing = velocity.x > 0 ? Facing::RIGHT : Facing::LEFT;
    } else if (fabsf(velocity.y) > 5.0f) {
        facing = velocity.y > 0 ? Facing::DOWN : Facing::UP;
    }
    
    // We moved this frame
    is_moving = (fabsf(velocity.x) > 5.0f || fabsf(velocity.y) > 5.0f);
}

} // namespace enemies 