/// entity.hpp — public interface for entity interactions
#pragma once
#include <raylib.h>

namespace core {
namespace entity {

// Entity position interface
Vector2 get_player_position();

// Damage interface
bool damage_player(int amount, Vector2 knockback_dir);

// Enemy detection interface - could be expanded as needed
bool is_enemy_at_position(float x, float y, float radius);

} // namespace entity
} // namespace core 