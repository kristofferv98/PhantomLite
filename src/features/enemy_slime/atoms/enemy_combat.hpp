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

/// Apply damage to the player when an enemy attacks
/// Returns true if damage was applied successfully
bool apply_player_damage(const Rectangle& attack_rect, int damage, Vector2 attack_pos);

/// Generate item drops when an enemy is defeated
void generate_drops(Vector2 position, const std::vector<enemies::DropChance>& drops);

} // namespace atoms
} // namespace enemy 
