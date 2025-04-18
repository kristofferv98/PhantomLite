# World Slice

The world slice manages the game environment, including tilemaps, camera system, and level loading.

## Context Primer

This slice handles everything related to the game world's environment and rendering:

- Tilemap loading and rendering for different biomes
- Camera management (following player, screen-to-world conversions)
- Physics/collision boundaries for the world
- Level loading from external map files
- Environment effects like weather and lighting

The world slice serves as the foundation for all other game entities that exist within its space. It manages coordinate transformations between world and screen space.

## Structure
- **atoms/**: Core functionalities (tilemap, camera)
- **world.hpp/cpp**: Public API (organism)

## Usage Example
```cpp
// Initialize and use world
world::init();
world::update(delta_time);
world::render();
world::set_camera_target(player_position);
bool can_walk = world::is_walkable(x, y);
Vector2 screen_pos = world::world_to_screen(world_pos);
```

## Key Concepts
- Tiles have properties (walkable, size)
- Multi-layer rendering for correct depth
- Camera follows target with smooth interpolation
- Coordinate systems: tile, world, and screen space

## Tile Types
- Grass: walkable base tile
- Dirt: walkable path
- Water: non-walkable
- Tree: 2x2 non-walkable
- Bush: non-walkable obstacle