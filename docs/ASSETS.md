# PhantomLite Asset Inventory

This document tracks all game assets, their status, and design notes to maintain consistency across the game.

## Tile Assets

### Basic Environment Tiles

| Asset | File | Status | Description |
|-------|------|--------|-------------|
| Grass | `assets_src/tiles/grass.svg` | ✅ Complete | Base ground tile with small flowers and variations |
| Dirt  | `assets_src/tiles/dirt.svg`  | ✅ Complete | Path tile with pebbles and texture - has transparent edges to blend with grass |
| Water | `assets_src/tiles/water.svg` | ✅ Complete | Water tile with waves and depth variations - transparent and blends well |

### Obstacle Tiles

| Asset | File | Status | Description |
|-------|------|--------|-------------|
| Tree | `assets_src/tiles/tree.svg` | ✅ Complete | 2x2 non-walkable tree obstacle with foliage and trunk - transparent background |
| Bush | `assets_src/tiles/bush.svg` | ✅ Complete | Non-walkable bush with berries - transparent background |

### Planned Environment Tiles

| Asset | File | Status | Description |
|-------|------|--------|-------------|
| Sand | - | 📝 Planned | Beach/desert sand tile with transparent edges |
| Rocks | - | 📝 Planned | Rocky ground tile with transparent background |
| Flowers | - | 📝 Planned | Decorative flower patch tile with transparent background |
| Snow | - | 📝 Planned | Snow-covered ground for winter areas with transparent edges |

### Planned Structure Tiles

| Asset | File | Status | Description |
|-------|------|--------|-------------|
| House | - | 📝 Planned | Basic house (2x2 or 3x3) with transparent background |
| Shop | - | 📝 Planned | Shop building with transparent background |
| Bridge | - | 📝 Planned | Bridge over water with transparent background |
| Sign | - | 📝 Planned | Signpost for directions with transparent background |
| Chest | - | 📝 Planned | Treasure chest with transparent background |

## Character Sprites

| Asset | File | Status | Description |
|-------|------|--------|-------------|
| Knight | `assets_src/sprites/knight.svg` | ✅ Complete | Main player character (idle) |
| Knight Walking | `assets_src/sprites/knight_walk.svg` | ✅ Complete | Knight walking animation frame |
| Knight Attack | `assets_src/sprites/knight_attack.svg` | ✅ Complete | Knight attack animation frame |

## Planned Character Sprites

| Asset | File | Status | Description |
|-------|------|--------|-------------|
| Knight Jump | - | 📝 Planned | Knight jumping animation |
| NPC Villager | - | 📝 Planned | Basic NPC character |
| Enemy Slime | - | 📝 Planned | Basic enemy type |
| Enemy Skeleton | - | 📝 Planned | Melee enemy type |

## Art Style Guidelines

### Color Palette

#### Environment
- **Grass Base**: `#58b546` (bright green)
- **Grass Accents**: `#6bc655` (light green), `#419c32` (dark green)
- **Dirt Base**: `#be8e5a` (tan)
- **Dirt Accents**: `#d2a97a` (light brown), `#9d7245` (dark brown)
- **Water Base**: `#4a94ce` (blue)
- **Water Accents**: `#6eaee6` (light blue), `#3678ae` (dark blue)

#### Characters
- **Knight**: (colors used in knight SVGs)
- **Enemies**: (planned color schemes)

### Design Principles

1. **Transparency is Required**: All object tiles MUST have transparent backgrounds to be properly layered
2. **Visual Clarity**: Objects should be immediately recognizable from their silhouette
3. **Consistency**: Use the established color palette for all assets
4. **Detail Level**: Include enough detail to be appealing but maintain the pixel-art inspired style
5. **Shadows**: Use subtle shadows to ground objects on their tiles

### SVG Transparency Guidelines

- **Never include opaque backgrounds** in SVG files for objects
- Use a small shadow ellipse under objects to ground them visually
- For terrain transitions (like dirt paths), use soft edges with transparency gradients
- Tree and bush assets should have completely transparent backgrounds
- Always test new assets in-game to verify proper transparency rendering

### SVG Technical Guidelines

- Use a 32x32 base size for 1x1 tiles
- Use 64x64 for 2x2 tiles
- Maintain consistent stroke widths (0.5-0.7 for small details)
- Use opacity and gradients sparingly for depth
- Include comments in SVG files to indicate structure

## Asset Pipeline

Our assets flow through this process:

1. Design `.svg` source files in `assets_src/` directory with transparent backgrounds
2. Automatic conversion to `.png` using the `convert_svgs.sh` script which preserves transparency
3. The game loads `.png` files at runtime and properly handles the alpha channel
4. Rendering code properly layers tiles, draws terrain before objects, and sorts objects by y-position

## Adding New Assets

When adding new assets:
1. Create the SVG file in the appropriate `assets_src/` subdirectory with transparent background
2. Update this document with the new asset
3. Run the conversion script
4. Verify transparency works properly in-game
5. If necessary, update tilemap code to use the new asset 