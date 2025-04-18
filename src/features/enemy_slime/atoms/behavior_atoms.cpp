/// behavior_atoms.cpp — Implementation of enemy behavior building blocks

#include "behavior_atoms.hpp"
#include "../enemy_slime.hpp"
#include "core/public/entity.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>

namespace enemy::atoms {

// PERF: Called once per enemy per frame - simple vector math, negligible cost
BehaviorResult wander_random(EnemyInstance& enemy, float dt) {
    WanderRandom& wander = enemy.wander;
    
    // Decrease timer
    if (wander.current_timer > 0) {
        wander.current_timer -= dt;
    }
    
    // If we don't have a target and timer expired, pick a new destination
    if (!wander.has_target && wander.current_timer <= 0) {
        // Random angle and distance with better distribution
        float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * PI;
        // Use square root for better radial distribution
        float distance_ratio = sqrtf(static_cast<float>(rand()) / RAND_MAX);
        float distance = distance_ratio * wander.radius;
        
        // Calculate new target position with more randomness
        wander.target.x = enemy.position.x + cosf(angle) * distance;
        wander.target.y = enemy.position.y + sinf(angle) * distance;
        wander.has_target = true;
        
        // Small random variance in idle time
        float idle_variance = static_cast<float>(rand()) / RAND_MAX * 0.4f + 0.8f; // 0.8 to 1.2
        wander.current_timer = 0.0f;
    }
    
    // If we have a target, move toward it with smooth acceleration
    if (wander.has_target) {
        // Calculate direction to target
        Vector2 direction = {
            wander.target.x - enemy.position.x,
            wander.target.y - enemy.position.y
        };
        
        // Calculate distance to target
        float distance = sqrtf(direction.x * direction.x + direction.y * direction.y);
        
        // If we're close enough to the target, mark as arrived
        if (distance < 5.0f) {
            wander.has_target = false;
            wander.current_timer = wander.idle_time;
            return BehaviorResult::Completed;
        }
        
        // Normalize direction
        if (distance > 0) {
            direction.x /= distance;
            direction.y /= distance;
        }
        
        // Slow down as we approach the target (ease-out effect)
        float speed_factor = 1.0f;
        if (distance < 50.0f) {
            speed_factor = 0.5f + (distance / 50.0f) * 0.5f;
        }
        
        // Move toward target with adjusted speed
        enemy.position.x += direction.x * enemy.spec->speed * speed_factor * dt;
        enemy.position.y += direction.y * enemy.spec->speed * speed_factor * dt;
        
        // Update collision rectangle
        enemy.collision_rect.x = enemy.position.x - enemy.collision_rect.width / 2;
        enemy.collision_rect.y = enemy.position.y - enemy.collision_rect.height / 2;
    }
    
    return BehaviorResult::Running;
}

// PERF: Called once per enemy per frame - distance check, ~1-2μs
BehaviorResult chase_player(EnemyInstance& enemy, float dt) {
    // Default random offset (wasn't in the original signature)
    Vector2 random_offset = {0, 0};
    
    ChasePlayer& chase = enemy.chase;
    
    // Get player position using core interface
    Vector2 player_pos = core::entity::get_player_position();
    
    // Calculate distance to player
    Vector2 to_player = {
        player_pos.x - enemy.position.x,
        player_pos.y - enemy.position.y
    };
    float distance_to_player = sqrtf(to_player.x * to_player.x + to_player.y * to_player.y);
    
    // Check if player is within detection radius
    if (distance_to_player <= chase.detection_radius) {
        chase.chasing = true;
    } else if (chase.chasing && distance_to_player > chase.detection_radius * 1.5f) {
        // Stop chasing if player gets too far (with hysteresis)
        chase.chasing = false;
        return BehaviorResult::Failed;
    }
    
    // If chasing, move toward player
    if (chase.chasing) {
        // Normalize direction
        if (distance_to_player > 0) {
            to_player.x /= distance_to_player;
            to_player.y /= distance_to_player;
        }
        
        // Adjust speed based on distance - faster when farther, slower when closer
        float speed_factor = 1.0f;
        float min_distance = 50.0f;  // Minimum approach distance
        
        // Slow down as we get closer to the player (for better gameplay)
        if (distance_to_player < chase.detection_radius * 0.5f) {
            // Ease out as we approach the player
            speed_factor = 0.6f + (distance_to_player / (chase.detection_radius * 0.5f)) * 0.4f;
        } else {
            // Slight speed boost when far away (catch-up mechanic)
            speed_factor = 1.0f + (distance_to_player / chase.detection_radius) * 0.3f;
        }
        
        // Never get too close to player (maintain minimum distance)
        if (distance_to_player > min_distance) {
            // Move toward player with adjusted speed
            enemy.position.x += (to_player.x * enemy.spec->speed * speed_factor * dt) + random_offset.x;
            enemy.position.y += (to_player.y * enemy.spec->speed * speed_factor * dt) + random_offset.y;
        }
        
        // Update collision rectangle
        enemy.collision_rect.x = enemy.position.x - enemy.collision_rect.width / 2;
        enemy.collision_rect.y = enemy.position.y - enemy.collision_rect.height / 2;
        
        return BehaviorResult::Running;
    }
    
    return BehaviorResult::Failed;
}

// PERF: Called once per enemy per frame - distance check and attack logic, ~2-4μs
BehaviorResult attack_player(EnemyInstance& enemy, float dt) {
    AttackPlayer& attack_data = enemy.attack;
    
    // Update cooldown timer
    if (!attack_data.can_attack) {
        attack_data.timer -= dt;
        if (attack_data.timer <= 0) {
            attack_data.can_attack = true;
            attack_data.timer = 0;
        }
    }
    
    // Get player position using core interface
    Vector2 player_pos = core::entity::get_player_position();
    
    // Calculate distance to player
    Vector2 to_player = {
        player_pos.x - enemy.position.x,
        player_pos.y - enemy.position.y
    };
    float distance_to_player = sqrtf(to_player.x * to_player.x + to_player.y * to_player.y);
    
    // If enemy is not in attack range, fail this behavior
    if (distance_to_player > attack_data.attack_radius) {
        attack_data.attacking = false;
        return BehaviorResult::Failed;
    }
    
    // If we can attack and player is in range, perform attack
    if (attack_data.can_attack) {
        // Calculate normalized knockback direction
        Vector2 knockback_dir = {0, 0};
        if (distance_to_player > 0) {
            knockback_dir.x = to_player.x / distance_to_player;
            knockback_dir.y = to_player.y / distance_to_player;
        }
        
        // Trace the attack for debugging
        TraceLog(LOG_INFO, "Enemy attacking player! Damage: %d", enemy.spec->dmg);
        
        // Try to apply damage to player with knockback direction using core interface
        bool hit = core::entity::damage_player(enemy.spec->dmg, knockback_dir);
        
        // Start cooldown
        attack_data.can_attack = false;
        attack_data.timer = attack_data.cooldown;
        attack_data.attacking = true;
        
        // Flash enemy color briefly to indicate attack
        enemy.color = RED; // Will be reset to GREEN in next update
        
        return BehaviorResult::Completed;
    }
    
    // Waiting for cooldown
    return BehaviorResult::Running;
}

} // namespace enemy::atoms 