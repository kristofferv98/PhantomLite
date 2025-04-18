# Enemies Common

## Context Primer
This slice contains the shared data structures and interfaces for all enemy types in the game. It defines the common enemy model, behavior atoms, and spawning interface to ensure consistency across all enemy archetypes. The system uses a 16-ray context steering grid for intelligent movement and obstacle avoidance.

## Structure
- `types.hpp/cpp` - Core data structures for enemy stats, runtime state, and behaviors
- `behavior_atoms.hpp/cpp` - Common behavior building blocks and steering system
- `spawn.hpp/cpp` - Interface for handling spawn requests and dispatching to type-specific factories

## Usage Example
```cpp
// Creating an enemy specification
enemies::EnemyStats slime_spec = {
    .id = enemies::EnemyID::FOR_SLIME,
    .hp = 2,
    .dmg = 1,
    // ...other properties
};

// Instantiating an enemy
enemies::EnemyRuntime slime(slime_spec, {100.0f, 200.0f});

// Using the context steering system
slime.reset_weights();
enemies::atoms::apply_seek_weights(slime, player_pos, 1.0f);
enemies::atoms::apply_obstacle_avoidance_weights(slime, 100.0f, 2.0f);
slime.apply_steering_movement(slime.spec->speed, dt);

// Processing a hit
enemies::Hit hit = {
    .dmg = 1,
    .knockback = {10.0f, 0.0f},
    .type = enemies::Hit::Type::Melee
};
slime.on_hit(hit);

// Debug visualization of steering
enemies::atoms::draw_steering_weights(slime);
```

## Key Concepts
- **EnemyStats** - Static data for each enemy type (HP, damage, behaviors)
- **EnemyRuntime** - Dynamic state for enemy instances with 16-ray steering grid
- **Context Steering** - Weight-based movement system using 16 directional rays
- **Behavior Atoms** - Composable behavior building blocks (wander, seek, strafe, etc.)
- **Weight Application** - Behaviors add/subtract from ray weights, then best direction is chosen

## Steering System
The steering system uses a 16-ray grid that samples directions in 22.5° increments around the enemy. Each behavior contributes weights to these rays:

1. **Seeking**: Applies positive weights toward target (proportional to dot product)
2. **Strafing**: Applies positive weights perpendicular to target direction
3. **Separation**: Applies positive weights away from nearby allies
4. **Obstacle Avoidance**: Applies negative weights toward obstacles

The final movement direction is chosen by selecting the ray with the highest positive weight, providing intelligent, emergent behavior from simple composable rules.

## Enemy Types
As defined in WORLDBUILDING.MD §5.3, the game has several enemy types organized by biome:
- Forest: FOR_SLIME, FOR_BOAR
- Cave: CAV_BAT
- Desert: DES_SCARAB
- Snow: SNW_WOLF
- Ruins: RUN_DRONE

Each specific enemy type is implemented in its own vertical slice, while sharing these common definitions. 