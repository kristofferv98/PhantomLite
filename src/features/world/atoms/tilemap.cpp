/// tilemap.cpp — implementation of tilemap atom
#include "tilemap.hpp"
#include <algorithm>
#include <cmath>

namespace world {
namespace atoms {

void Tilemap::init(int map_width, int map_height, int tile_size) {
    width_ = map_width;
    height_ = map_height;
    tile_size_ = tile_size;
    
    // Initialize with empty tiles
    tiles_.resize(width_ * height_, TileType::NONE);
    
    // Set up tile properties
    init_properties();
}

bool Tilemap::load_textures() {
    // Load textures for each tile type with proper transparency
    Image img;
    
    // Load grass (base tile)
    img = LoadImage("assets/tiles/grass.png");
    textures_[TileType::GRASS] = LoadTextureFromImage(img);
    UnloadImage(img);
    
    // Load dirt with transparency
    img = LoadImage("assets/tiles/dirt.png");
    textures_[TileType::DIRT] = LoadTextureFromImage(img);
    UnloadImage(img);
    
    // Load water with transparency
    img = LoadImage("assets/tiles/water.png");
    textures_[TileType::WATER] = LoadTextureFromImage(img);
    UnloadImage(img);
    
    // Load tree with transparency
    img = LoadImage("assets/tiles/tree.png");
    textures_[TileType::TREE] = LoadTextureFromImage(img);
    UnloadImage(img);
    
    // Load bush with transparency
    img = LoadImage("assets/tiles/bush.png");
    textures_[TileType::BUSH] = LoadTextureFromImage(img);
    UnloadImage(img);
    
    // Check if textures loaded correctly
    for (const auto& pair : textures_) {
        if (pair.second.id == 0) {
            TraceLog(LOG_WARNING, "Failed to load texture for tile type %d", static_cast<int>(pair.first));
            return false;
        }
    }
    
    return true;
}

void Tilemap::generate_demo_map() {
    // Fill the map with grass by default
    std::fill(tiles_.begin(), tiles_.end(), TileType::GRASS);
    
    // Create a simple dirt path
    for (int x = 10; x < width_ - 10; x++) {
        set_tile(x, height_ / 2, TileType::DIRT);
        set_tile(x, height_ / 2 + 1, TileType::DIRT);
    }
    
    // Add some water
    for (int y = 5; y < 15; y++) {
        for (int x = 5; x < 15; x++) {
            set_tile(x, y, TileType::WATER);
        }
    }
    
    // Add some trees (remember trees are 2x2 tiles)
    set_tile(20, 10, TileType::TREE);
    set_tile(25, 15, TileType::TREE);
    set_tile(15, 20, TileType::TREE);
    set_tile(30, 5, TileType::TREE);
    
    // Add some bushes
    set_tile(18, 8, TileType::BUSH);
    set_tile(22, 8, TileType::BUSH);
    set_tile(12, 18, TileType::BUSH);
    set_tile(16, 22, TileType::BUSH);
    set_tile(28, 22, TileType::BUSH);
    set_tile(32, 10, TileType::BUSH);
    set_tile(10, 28, TileType::BUSH);
    set_tile(35, 25, TileType::BUSH);
}

void Tilemap::set_tile(int x, int y, TileType type) {
    // Check bounds
    if (x < 0 || x >= width_ || y < 0 || y >= height_) {
        return;
    }
    
    // For large tiles (like trees), make sure we have space
    const TileProperties& props = get_tile_properties(type);
    if (props.is_large) {
        // Check if we have enough space
        for (int dy = 0; dy < props.height_in_tiles; dy++) {
            for (int dx = 0; dx < props.width_in_tiles; dx++) {
                int check_x = x + dx;
                int check_y = y + dy;
                
                // Skip if out of bounds
                if (check_x >= width_ || check_y >= height_) {
                    return;
                }
                
                // For large tiles, only set the top-left tile to the actual type
                // The other tiles in the area will be set to NONE to mark them as "occupied"
                if (dx == 0 && dy == 0) {
                    tiles_[check_y * width_ + check_x] = type;
                } else {
                    tiles_[check_y * width_ + check_x] = TileType::NONE;
                }
            }
        }
    } else {
        // Regular 1x1 tile
        tiles_[y * width_ + x] = type;
    }
}

TileType Tilemap::get_tile(int x, int y) const {
    // Check bounds
    if (x < 0 || x >= width_ || y < 0 || y >= height_) {
        return TileType::NONE;
    }
    
    return tiles_[y * width_ + x];
}

bool Tilemap::is_walkable(int x, int y) const {
    TileType tile = get_tile(x, y);
    
    // Special case for NONE - need to check if it's part of a large tile
    if (tile == TileType::NONE) {
        // Check surrounding tiles to see if we're part of a large tile
        for (int dy = -1; dy <= 0; dy++) {
            for (int dx = -1; dx <= 0; dx++) {
                int check_x = x + dx;
                int check_y = y + dy;
                
                // Skip if out of bounds
                if (check_x < 0 || check_x >= width_ || check_y < 0 || check_y >= height_) {
                    continue;
                }
                
                TileType nearby = tiles_[check_y * width_ + check_x];
                const TileProperties& props = get_tile_properties(nearby);
                
                // If it's a large tile and we're within its bounds, use its walkability
                if (props.is_large) {
                    int rel_x = x - check_x;
                    int rel_y = y - check_y;
                    
                    if (rel_x >= 0 && rel_x < props.width_in_tiles && 
                        rel_y >= 0 && rel_y < props.height_in_tiles) {
                        return props.walkable;
                    }
                }
            }
        }
        
        // No large tile found, default to true for NONE
        return true;
    }
    
    // Regular tile, use its properties
    return get_tile_properties(tile).walkable;
}

bool Tilemap::is_walkable(float world_x, float world_y) const {
    // Convert world coordinates to tile coordinates
    Vector2 tile_pos = world_to_tile(world_x, world_y);
    return is_walkable(static_cast<int>(tile_pos.x), static_cast<int>(tile_pos.y));
}

void Tilemap::render(const Rectangle& camera_view) {
    // Calculate visible tile range
    int start_x = std::max(0, static_cast<int>(camera_view.x / tile_size_));
    int start_y = std::max(0, static_cast<int>(camera_view.y / tile_size_));
    int end_x = std::min(width_, static_cast<int>((camera_view.x + camera_view.width) / tile_size_) + 1);
    int end_y = std::min(height_, static_cast<int>((camera_view.y + camera_view.height) / tile_size_) + 1);
    
    // STEP 1: Draw the base ground tiles first (GRASS only)
    for (int y = start_y; y < end_y; y++) {
        for (int x = start_x; x < end_x; x++) {
            float draw_x = x * tile_size_;
            float draw_y = y * tile_size_;
            
            // Always draw grass as the base layer
            DrawTexture(textures_[TileType::GRASS], 
                       static_cast<int>(draw_x - camera_view.x),
                       static_cast<int>(draw_y - camera_view.y),
                       WHITE);
        }
    }
    
    // STEP 2: Draw all flat terrain tiles on top of grass (WATER, DIRT)
    for (int y = start_y; y < end_y; y++) {
        for (int x = start_x; x < end_x; x++) {
            TileType tile = get_tile(x, y);
            
            if (tile == TileType::WATER || tile == TileType::DIRT) {
                float draw_x = x * tile_size_;
                float draw_y = y * tile_size_;
                
                DrawTexture(textures_[tile], 
                           static_cast<int>(draw_x - camera_view.x),
                           static_cast<int>(draw_y - camera_view.y),
                           WHITE);
            }
        }
    }
    
    // STEP 3: Prepare a list of objects to be drawn with their y-positions for proper layering
    struct DrawableObject {
        TileType type;
        float x, y;
        float sort_y; // Y-position for sorting (bottom of object)
    };
    std::vector<DrawableObject> objects_to_draw;
    
    // Collect all objects that need to be drawn on top
    for (int y = start_y; y < end_y; y++) {
        for (int x = start_x; x < end_x; x++) {
            TileType tile = get_tile(x, y);
            
            if (tile == TileType::TREE || tile == TileType::BUSH) {
                const TileProperties& props = get_tile_properties(tile);
                
                // For large objects, only add them once from their top-left position
                if (props.is_large) {
                    // Check if this is the top-left of a large object
                    bool is_top_left = true;
                    for (int dy = -1; dy <= 0 && is_top_left; dy++) {
                        for (int dx = -1; dx <= 0 && is_top_left; dx++) {
                            if (dx == 0 && dy == 0) continue; // Skip self
                            
                            int check_x = x + dx;
                            int check_y = y + dy;
                            
                            // Skip out of bounds
                            if (check_x < 0 || check_x >= width_ || check_y < 0 || check_y >= height_) {
                                continue;
                            }
                            
                            if (get_tile(check_x, check_y) == tile) {
                                is_top_left = false;
                                break;
                            }
                        }
                    }
                    
                    if (is_top_left) {
                        float draw_x = x * tile_size_;
                        float draw_y = y * tile_size_;
                        // Bottom of the object for sorting (bottom of the entire object)
                        float sort_y = draw_y + (props.height_in_tiles * tile_size_);
                        
                        objects_to_draw.push_back({tile, draw_x, draw_y, sort_y});
                    }
                } else {
                    // Regular 1x1 tile
                    float draw_x = x * tile_size_;
                    float draw_y = y * tile_size_;
                    // For 1x1 tiles, sort by bottom edge
                    float sort_y = draw_y + tile_size_;
                    
                    objects_to_draw.push_back({tile, draw_x, draw_y, sort_y});
                }
            }
        }
    }
    
    // STEP 4: Sort objects by their bottom y-coordinate for proper depth layering
    std::sort(objects_to_draw.begin(), objects_to_draw.end(), 
             [](const DrawableObject& a, const DrawableObject& b) {
                 return a.sort_y < b.sort_y;
             });
    
    // STEP 5: Draw all objects in sorted order
    for (const auto& obj : objects_to_draw) {
        DrawTexture(textures_[obj.type], 
                   static_cast<int>(obj.x - camera_view.x),
                   static_cast<int>(obj.y - camera_view.y),
                   WHITE);
    }
}

void Tilemap::cleanup() {
    // Unload all textures
    for (auto& pair : textures_) {
        UnloadTexture(pair.second);
    }
    textures_.clear();
}

Vector2 Tilemap::world_to_tile(float world_x, float world_y) const {
    return {
        std::floor(world_x / tile_size_),
        std::floor(world_y / tile_size_)
    };
}

Vector2 Tilemap::tile_to_world(int tile_x, int tile_y) const {
    return {
        static_cast<float>(tile_x * tile_size_),
        static_cast<float>(tile_y * tile_size_)
    };
}

const TileProperties& Tilemap::get_tile_properties(TileType type) const {
    auto it = properties_.find(type);
    if (it != properties_.end()) {
        return it->second;
    }
    
    // Default empty property as fallback
    static const TileProperties empty_props = { true, false, 1, 1 };
    return empty_props;
}

void Tilemap::init_properties() {
    // Initialize properties for each tile type
    properties_[TileType::NONE] = { true, false, 1, 1 };
    properties_[TileType::GRASS] = { true, false, 1, 1 };
    properties_[TileType::DIRT] = { true, false, 1, 1 };
    properties_[TileType::WATER] = { false, false, 1, 1 };
    properties_[TileType::TREE] = { false, true, 2, 2 };
    properties_[TileType::BUSH] = { false, false, 1, 1 };
}

} // namespace atoms
} // namespace world 