/// enemy_combat.hpp — Enemy combat interactions atom

#pragma once

#include "../enemy_slime.hpp"
#include <raylib.h>

namespace enemy {
namespace atoms {

/// Initialize the combat system
void init_combat();

/// Check if an attack at a position hits any enemies
/// Returns true if a hit is registered
bool hit_enemy_at(const Rectangle& hit_rect, const enemies::Hit& hit);

/// Generate item drops when an enemy is defeated
void generate_drops(Vector2 position, const std::array<DropChance, 2>& drops);

} // namespace atoms
} // namespace enemy 