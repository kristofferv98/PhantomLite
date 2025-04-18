# PhantomLite

A modern 2D pixel-art style game engine with Zelda-like mechanics, built with C++ and Raylib.

![PhantomLite Screenshot](docs/images/screenshot.png)

## Features

- Modular architecture using atomic design principles
- Dynamic collision detection system with multiple shape types
- Camera system with smooth following
- SVG-based asset pipeline with transparency support
- Tile-based world system with proper layering
- Animation system with state transitions

## Project Architecture

PhantomLite uses atomic design principles for code organization:

- **Atoms**: Small, reusable components with single responsibility (≤250 LOC)
- **Molecules**: Composed of multiple atoms to provide larger functionality 
- **Organisms**: Top-level public APIs that external code interacts with

## Project Structure

```
/                        (git root)
├─ conanfile.txt         ── external dependencies
├─ CMakePresets.json     ── reproducible build configs
│
├─ src/
│   ├─ core/             ── engine loop, renderer bootstrap, platform glue
│   ├─ features/         ── vertical slices (each game system)
│   │    ├─ player/      ── player movement, animation, collision
│   │    ├─ world/       ── tilemap, camera system 
│   │    ├─ ui/          ── HUD, menus, health display
│   │    └─ enemy_*/     ── enemy AI behaviors (per type)
│   └─ shared/           ── pure helpers used by ≥2 slices
│
├─ assets_src/
│   ├─ sprites/          ── SVG source files for characters
│   ├─ tiles/            ── SVG source files for terrain
│   └─ ui/               ── SVG source files for interface elements
│
├─ tools/                ── asset pipeline scripts
│   └─ convert_svgs.sh   ── SVG to PNG conversion tool
│
└─ docs/                 ── documentation
    ├─ PROGRESS.md       ── project tracking
    ├─ ASSETS.md         ── asset inventory
    └─ WORLDBUILDING.MD  ── game lore and mechanics
```

## Building the Project

### Prerequisites

- C++17 compatible compiler (GCC 13+, Clang 15+, or MSVC 2019+)
- CMake 3.15+
- Conan 2.0+ (for dependencies)

### Dependencies

- Raylib (graphics and input handling)
- fmt (modern string formatting)
- Catch2 (for testing)

### Build Instructions

1. Install dependencies:
```bash
conan profile detect --force
conan install . --output-folder=build --build=missing
```

2. Configure and build:
```bash
cmake --preset=default
cmake --build --preset=default --target game
```

3. Run the game:
```bash
./build/game
```

## Controls

- Arrow keys: Move player
- Space: Attack
- C: Toggle collision visualization

## Asset Pipeline

The project uses an SVG-to-PNG asset pipeline:

1. Source assets are created as SVG files in `assets_src/` directories
2. Run `tools/convert_svgs.sh` to generate PNG files in `assets/`
3. The game loads these PNG files at runtime with transparency support


## Documentation

- [Progress Tracking](docs/PROGRESS.md) - Current project status and roadmap
- [Asset Inventory](docs/ASSETS.md) - List of all game assets
- [Worldbuilding](docs/WORLDBUILDING.MD) - Game lore, regions, and enemy types

## License

This project is dual-licensed under:
- MIT License for code (see LICENSE file)
- CC-BY-SA 4.0 for assets

## Acknowledgments

- Inspired by classic Zelda games
- Using atomic design principles for game development