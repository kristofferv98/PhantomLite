/// enemy_spawner.hpp — Enemy spawning atom

#pragma once

#include "../enemy_slime.hpp"
#include <raylib.h>

namespace enemy {
namespace atoms {

/// Initialize the enemy spawner
void init_spawner();

/// Spawn a slime at the specified position
void spawn_slime_at(Vector2 position);

/// Spawn multiple slimes for demo purposes
/// This will spawn slimes at random positions around the map
void spawn_demo_slimes(int count);

} // namespace atoms
} // namespace enemy 