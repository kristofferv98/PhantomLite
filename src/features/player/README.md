# Player Slice

The player slice manages all aspects of the player character including movement, combat, animation, and health.

## Context Primer

This slice handles player-related functionality, serving as the central point for player gameplay experience:

- Movement and input handling
- Combat mechanics (attacking, damage, knockback)
- Animation system for player states (idle, walking, attacking)
- Health management and hearts display
- Collision detection with world and enemies
- Player progression (inventory integration)

The player slice uses a controller-based system where different aspects of player functionality are organized into molecules and atoms for better code organization and reuse.

## Structure
- **atoms/**: Core functionalities (movement, animation, collision, actions, debug)
- **molecules/**: Compositions of atoms (controller)
- **player.hpp/cpp**: Public API (organism)

## Usage Example
```cpp
// Initialize and use player
player::init();
player::update(delta_time);
player::render();
player::cleanup();
```

## Key Concepts
- Player has three states: IDLE, WALKING, ATTACKING
- Collision uses rectangle and circle shapes
- Debug visualization toggled with 'C' key
- Movement controlled by arrow keys, attack with space

## Data Flow
Input → Movement/Actions → Animation State → Position Update → Collision Resolution → Rendering