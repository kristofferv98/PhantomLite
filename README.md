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

## Project Structure

```
/                 (git root)
├─ conanfile.txt          ── external dependencies
├─ CMakePresets.json      ── reproducible build configs
│
├─ src/
│   ├─ core/              ── engine loop, renderer bootstrap, platform glue
│   ├─ features/          ── vertical slices (each game system)
│   │    ├─ player/
│   │    │    ├─ player.hpp / .cpp  (organism)
│   │    │    ├─ atoms/             (isolated funcs/classes ≤150 LOC)
│   │    │    ├─ molecules/         (2–3 related atoms)
│   │    │    └─ ...
│   │    ├─ world/
│   │    └─ ...
│   └─ shared/            ── pure helpers used by ≥2 slices
│
├─ assets_src/
│   ├─ sprites/           (.svg source files)
│   └─ tiles/             (.svg source files)
│
├─ tools/                 (asset pipeline scripts)
│
└─ docs/                  (documentation)
    ├─ PROGRESS.md        (project tracking)
    └─ ASSETS.md          (asset inventory)
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
./build/src/game
```

## Controls

- Arrow keys: Move player
- Space: Attack
- C: Toggle collision visualization

## Asset Pipeline

The project uses an SVG-to-PNG asset pipeline:

1. Source assets are created as SVG files in `assets_src/`
2. Run `tools/convert_svgs.sh` to generate PNG files in `assets/`
3. The game loads these PNG files at runtime

## Documentation

- [Progress Tracking](docs/PROGRESS.md) - Current project status
- [Asset Inventory](docs/ASSETS.md) - List of all game assets and design guidelines

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Inspired by classic Zelda games
- Using atomic design principles for game development 