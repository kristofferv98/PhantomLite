# Enemies Common

## Context Primer
This slice contains the shared data structures and interfaces for all enemy types in the game. It defines the common enemy model, behavior atoms, and spawning interface to ensure consistency across all enemy archetypes.

## Structure
- `types.hpp/cpp` - Core data structures for enemy stats, runtime state, and behaviors
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

// Processing a hit
enemies::Hit hit = {
    .dmg = 1,
    .knockback = {10.0f, 0.0f},
    .type = enemies::Hit::Type::Melee
};
```

## Key Concepts
- **EnemyStats** - Static data for each enemy type (HP, damage, behaviors)
- **EnemyRuntime** - Dynamic state for enemy instances (position, current HP)
- **Behavior Atoms** - Composable behavior building blocks (wander, chase, attack)
- **Spawn Request** - Data structure for enemy spawning from level JSON

## Enemy Types
As defined in WORLDBUILDING.MD §5.3, the game has several enemy types organized by biome:
- Forest: FOR_SLIME, FOR_BOAR
- Cave: CAV_BAT
- Desert: DES_SCARAB
- Snow: SNW_WOLF
- Ruins: RUN_DRONE

Each specific enemy type is implemented in its own vertical slice, while sharing these common definitions. 