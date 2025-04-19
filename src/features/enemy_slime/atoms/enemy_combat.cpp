/// enemy_combat.cpp — Enemy combat interactions atom implementation

#include "enemy_combat.hpp"
#include "enemy_state.hpp"
#include "../../player/player.hpp"
#include <random>
#include <cmath>

namespace enemy {
namespace atoms {

void init_combat() {
    // No specific initialization needed
}

// PERF: ~0.05ms for a typical hit check (varies with enemy count)
bool hit_enemy_at(const Rectangle& hit_rect, const enemies::Hit& hit) {
    // Delegate to the state manager to apply damage
    return apply_damage_at(hit_rect, hit);
}

/// Apply damage to the player when an enemy attacks
/// Returns true if damage was applied successfully
bool apply_player_damage(const Rectangle& attack_rect, int damage, Vector2 attack_pos) {
    // Get player hitbox/collision rectangle
    Vector2 player_pos = player::get_position();
    Rectangle player_rect = {
        player_pos.x - 16, player_pos.y - 16, // Approximate player hitbox
        32, 32
    };
    
    // Check collision
    if (CheckCollisionRecs(attack_rect, player_rect)) {
        // Calculate knockback direction (away from enemy)
        Vector2 knockback_dir = {
            player_pos.x - attack_pos.x,
            player_pos.y - attack_pos.y
        };
        
        // Normalize knockback direction
        float len = sqrtf(knockback_dir.x * knockback_dir.x + knockback_dir.y * knockback_dir.y);
        if (len > 0) {
            knockback_dir.x /= len;
            knockback_dir.y /= len;
        } else {
            knockback_dir.x = 1.0f; // Default direction if at same position
            knockback_dir.y = 0.0f;
        }
        
        // Apply damage to player with calculated knockback direction
        return player::take_damage(damage, knockback_dir);
    }
    
    return false;
}

void generate_drops(Vector2 position, const std::vector<enemies::DropChance>& drops) {
    // Create random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(1, 100); // 1-100 for percentage check
    
    // Check each drop type
    for (const auto& drop : drops) {
        int roll = dis(gen);
        
        // If roll is within the drop chance, spawn the item
        if (roll <= drop.chance) {
            // Determine what to drop
            switch (drop.type) {
                case enemies::DropType::Heart:
                    TraceLog(LOG_INFO, "Enemy dropped: Heart");
                    // TODO: Implement actual heart pickup spawning
                    break;
                    
                case enemies::DropType::Coin:
                    TraceLog(LOG_INFO, "Enemy dropped: Coin");
                    // TODO: Implement actual coin pickup spawning
                    break;
                    
                default:
                    break;
            }
        }
    }
}

} // namespace atoms
} // namespace enemy 
