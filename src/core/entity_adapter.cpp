/// entity_adapter.cpp — implementation of entity interfaces
#include "public/entity.hpp"
#include "../features/player/player.hpp"

namespace core {
namespace entity {

Vector2 get_player_position() {
    // Delegate to player module
    return player::get_position();
}

bool damage_player(int amount, Vector2 knockback_dir) {
    // Delegate to player module
    return player::take_damage(amount, knockback_dir);
}

bool is_enemy_at_position(float x, float y, float radius) {
    // This would need to check with enemy manager
    // For now, stub implementation that always returns false
    return false;
}

} // namespace entity
} // namespace core 