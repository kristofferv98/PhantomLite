/// enemy_combat.cpp — Enemy combat interactions atom implementation

#include "enemy_combat.hpp"
#include "enemy_state.hpp"
#include <random>

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