/// enemy_combat.hpp — Enemy combat interactions atom

#pragma once

#include "../enemy_slime.hpp"
#include <raylib.h>
#include "../../enemies/types.hpp"
#include <vector>

namespace enemy {
namespace atoms {

/// Initialize the combat system
void init_combat();

/// Check if a hit rectangle intersects with an enemy
/// Returns true if any enemy was hit
bool hit_enemy_at(const Rectangle& hit_rect, const enemies::Hit& hit);

/// Apply damage to the player when an enemy attack hits
/// Returns true if damage was successfully applied
bool apply_player_damage(const Rectangle& attack_rect, int damage);

/// Generate drops at the specified position based on drop chances
void generate_drops(Vector2 position, const std::vector<enemies::DropChance>& drops);

} // namespace atoms
} // namespace enemy 