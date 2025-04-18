/// types.cpp — Implementation of common enemy data structures

#include "types.hpp"
#include "../world/world.hpp"
#include <cmath>
#include <algorithm>

namespace enemies {

// Constructor implementation for EnemyRuntime
EnemyRuntime::EnemyRuntime(const EnemyStats& spec_ref, Vector2 pos)
    : spec(&spec_ref), 
      position(pos), 
      hp(spec_ref.hp), 
      collision_rect({pos.x - spec_ref.size.x/2, pos.y - spec_ref.size.y/2, spec_ref.size.x, spec_ref.size.y}),
      color(GREEN),
      facing(Facing::DOWN),
      active(true),
      anim_timer(0.0f),
      anim_frame(0),
      is_moving(false) {
    
    // Initialize chase parameters with values from spec
    chase.detection_radius = spec_ref.detection_radius;
    chase.chasing = false;
    
    // Initialize attack parameters with values from spec
    attack.attack_radius = spec_ref.attack_radius;
    attack.cooldown = spec_ref.attack_cooldown;
    attack.timer = 0.0f;
    attack.can_attack = true;
    attack.attacking = false;
    
    // Initialize melee attack parameters
    attack_melee.reach = spec_ref.attack_radius;
    attack_melee.cooldown = spec_ref.attack_cooldown;
    
    // Initialize new noise-based wander
    wander_noise.spawn_point = pos;
    
    // Initialize with random noise offsets
    wander_noise.noise_offset_x = static_cast<float>(rand()) / RAND_MAX * 1000.0f;
    wander_noise.noise_offset_y = static_cast<float>(rand()) / RAND_MAX * 1000.0f;
    
    // Initialize weights array to zeros
    reset_weights();
}

// Handle being hit by an attack
void EnemyRuntime::on_hit(const Hit& hit) {
    // Early exit if already dead
    if (!is_alive()) return;
    
    // Apply damage
    hp -= hit.dmg;
    
    // Apply knockback if any
    if (hit.knockback.x != 0 || hit.knockback.y != 0) {
        // Simple knockback - could be more complex with physics
        position.x += hit.knockback.x * 10.0f;
        position.y += hit.knockback.y * 10.0f;
        
        // Update collision rectangle
        collision_rect.x = position.x - spec->size.x/2;
        collision_rect.y = position.y - spec->size.y/2;
    }
    
    // Flash color to indicate hit
    color = RED;
    
    // TODO: Special reactions based on hit type
    switch (hit.type) {
        case Hit::Type::Pierce:
            // Pierce might ignore armor or do more damage
            break;
            
        case Hit::Type::Magic:
            // Magic might have special effects
            break;
            
        default:
            break;
    }
    
    // Check if dead
    if (hp <= 0) {
        // Mark as inactive
        active = false;
    }
}

// Apply movement based on steering weights
void EnemyRuntime::apply_steering_movement(float speed, float dt) {
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
    Vector2 move_dir = get_ray_dir(best_ray);
    
    // Apply movement
    float adjusted_speed = speed * dt;
    position.x += move_dir.x * adjusted_speed;
    position.y += move_dir.y * adjusted_speed;
    
    // Update collision rectangle
    collision_rect.x = position.x - spec->size.x/2;
    collision_rect.y = position.y - spec->size.y/2;
    
    // Update facing direction based on movement
    if (fabsf(move_dir.x) > fabsf(move_dir.y)) {
        facing = move_dir.x > 0 ? Facing::RIGHT : Facing::LEFT;
    } else {
        facing = move_dir.y > 0 ? Facing::DOWN : Facing::UP;
    }
    
    // We moved this frame
    is_moving = true;
}

} // namespace enemies 