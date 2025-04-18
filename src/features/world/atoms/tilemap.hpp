/// tilemap.hpp — tilemap atom for world slice
#pragma once
#include <raylib.h>
#include <vector>
#include <string>
#include <unordered_map>

namespace world {
namespace atoms {

// Tile types
enum class TileType {
    NONE,     // Empty/no tile
    GRASS,    // Walkable grass
    DIRT,     // Walkable dirt path
    WATER,    // Non-walkable water
    TREE,     // Non-walkable tree (2x2 tile)
    BUSH      // Non-walkable bush
};

// Tile properties
struct TileProperties {
    bool walkable;       // Can player walk on this tile?
    bool is_large;       // Is this a larger than 1x1 tile (like tree)?
    int width_in_tiles;  // Width in tile units
    int height_in_tiles; // Height in tile units
};

// Tilemap handles loading, rendering, and collision for tiles
class Tilemap {
public:
    // Initialize the tilemap
    void init(int map_width, int map_height, int tile_size);
    
    // Load tile textures
    bool load_textures();
    
    // Generate a simple demo map
    void generate_demo_map();
    
    // Set a tile at position
    void set_tile(int x, int y, TileType type);
    
    // Get tile at position
    TileType get_tile(int x, int y) const;
    
    // Check if position is walkable
    bool is_walkable(int x, int y) const;
    bool is_walkable(float world_x, float world_y) const;
    
    // Render the visible portion of the tilemap
    void render(const Rectangle& camera_view);
    
    // Clean up resources
    void cleanup();
    
    // Get tile properties
    const TileProperties& get_tile_properties(TileType type) const;
    
    // Convert world coordinates to tile coordinates and vice versa
    Vector2 world_to_tile(float world_x, float world_y) const;
    Vector2 tile_to_world(int tile_x, int tile_y) const;
    
    // Map dimensions
    int get_width() const { return width_; }
    int get_height() const { return height_; }
    int get_tile_size() const { return tile_size_; }
    
private:
    std::vector<TileType> tiles_;
    std::unordered_map<TileType, Texture2D> textures_;
    std::unordered_map<TileType, TileProperties> properties_;
    
    int width_ = 0;
    int height_ = 0;
    int tile_size_ = 32;
    
    // Initialize tile properties
    void init_properties();
};

} // namespace atoms
} // namespace world 