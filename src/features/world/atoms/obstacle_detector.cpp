/// obstacle_detector.cpp — Implementation of obstacle detection utility
#include "obstacle_detector.hpp"
#include "tilemap.hpp"
#include "../../shared/math_utils.hpp"
#include <algorithm>
#include <cmath>

namespace world {
namespace atoms {

void ObstacleDetector::init(Tilemap* tilemap) {
    tilemap_ = tilemap;
}

RaycastHit ObstacleDetector::raycast(Vector2 origin, Vector2 direction, float max_distance) const {
    RaycastHit result = {false, max_distance, {0, 0}, {0, 0}};
    
    if (!tilemap_) return result;
    
    // Convert origin to tile coordinates
    Vector2 tile_pos = tilemap_->world_to_tile(origin.x, origin.y);
    int startTileX = static_cast<int>(tile_pos.x);
    int startTileY = static_cast<int>(tile_pos.y);
    
    // Normalize direction for DDA algorithm
    float len = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (len > 0) {
        direction.x /= len;
        direction.y /= len;
    } else {
        return result; // Invalid direction
    }
    
    // DDA algorithm for raycasting on a grid
    float posX = origin.x;
    float posY = origin.y;
    
    // Calculate step size for DDA
    float stepSize = tilemap_->get_tile_size() * 0.5f; // Half tile size for better accuracy
    
    for (float dist = 0; dist < max_distance; dist += stepSize) {
        // Move along ray
        posX += direction.x * stepSize;
        posY += direction.y * stepSize;
        
        // Convert to tile coordinates
        Vector2 current_tile = tilemap_->world_to_tile(posX, posY);
        int tileX = static_cast<int>(current_tile.x);
        int tileY = static_cast<int>(current_tile.y);
        
        // Check tile bounds
        if (tileX < 0 || tileX >= tilemap_->get_width() || 
            tileY < 0 || tileY >= tilemap_->get_height()) {
            // Ray went out of bounds
            result.hit = true;
            result.distance = dist;
            result.point = {posX, posY};
            
            // Calculate normal (always away from world bounds)
            result.normal = {0, 0};
            if (tileX < 0) result.normal.x = 1;
            else if (tileX >= tilemap_->get_width()) result.normal.x = -1;
            if (tileY < 0) result.normal.y = 1;
            else if (tileY >= tilemap_->get_height()) result.normal.y = -1;
            
            break;
        }
        
        // Check if current tile is an obstacle
        if (!tilemap_->is_walkable(tileX, tileY)) {
            result.hit = true;
            result.distance = dist;
            result.point = {posX, posY};
            
            // Calculate normal based on which edge we hit
            // Simple approximation - better normal calculation would require more complex logic
            Vector2 tileCenterWorld = tilemap_->tile_to_world(tileX, tileY);
            tileCenterWorld.x += tilemap_->get_tile_size() / 2.0f;
            tileCenterWorld.y += tilemap_->get_tile_size() / 2.0f;
            
            // Calculate direction from tile center to hit point
            Vector2 fromCenter = {
                result.point.x - tileCenterWorld.x,
                result.point.y - tileCenterWorld.y
            };
            
            // Normalize to get normal
            float normalLen = std::sqrt(fromCenter.x * fromCenter.x + fromCenter.y * fromCenter.y);
            if (normalLen > 0) {
                result.normal.x = fromCenter.x / normalLen;
                result.normal.y = fromCenter.y / normalLen;
            } else {
                // Fallback if at exact center (unlikely)
                result.normal = {-direction.x, -direction.y};
            }
            
            break;
        }
    }
    
    return result;
}

bool ObstacleDetector::check_circle_overlap(Vector2 center, float radius) const {
    if (!tilemap_) return false;
    
    // Convert center to tile coordinates
    Vector2 tile_pos = tilemap_->world_to_tile(center.x, center.y);
    int centerTileX = static_cast<int>(tile_pos.x);
    int centerTileY = static_cast<int>(tile_pos.y);
    
    // Calculate tile radius in grid cells (ceiling to ensure we check all overlapped tiles)
    int tileRadius = static_cast<int>(std::ceil(radius / tilemap_->get_tile_size()));
    
    // Check all tiles that might overlap with the circle
    for (int y = centerTileY - tileRadius; y <= centerTileY + tileRadius; y++) {
        for (int x = centerTileX - tileRadius; x <= centerTileX + tileRadius; x++) {
            // Skip if out of bounds
            if (x < 0 || x >= tilemap_->get_width() || y < 0 || y >= tilemap_->get_height())
                continue;
            
            if (!tilemap_->is_walkable(x, y)) {
                // Calculate tile center in world space
                Vector2 tileCenter = tilemap_->tile_to_world(x, y);
                tileCenter.x += tilemap_->get_tile_size() / 2.0f;
                tileCenter.y += tilemap_->get_tile_size() / 2.0f;
                
                // Calculate distance from circle center to tile center
                float dist = shared::math::distance(center, tileCenter);
                
                // Check if circle overlaps with tile
                // This is an approximation - assumes square tiles are circles for simplicity
                if (dist < radius + tilemap_->get_tile_size() / 2.0f) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

bool ObstacleDetector::check_rect_overlap(Rectangle rect) const {
    if (!tilemap_) return false;
    
    // Calculate tile coordinates for rectangle corners
    Vector2 topLeft = tilemap_->world_to_tile(rect.x, rect.y);
    Vector2 bottomRight = tilemap_->world_to_tile(rect.x + rect.width, rect.y + rect.height);
    
    int minTileX = static_cast<int>(topLeft.x);
    int minTileY = static_cast<int>(topLeft.y);
    int maxTileX = static_cast<int>(bottomRight.x);
    int maxTileY = static_cast<int>(bottomRight.y);
    
    // Check all tiles that might overlap with the rectangle
    for (int y = minTileY; y <= maxTileY; y++) {
        for (int x = minTileX; x <= maxTileX; x++) {
            // Skip if out of bounds
            if (x < 0 || x >= tilemap_->get_width() || y < 0 || y >= tilemap_->get_height())
                continue;
            
            if (!tilemap_->is_walkable(x, y)) {
                return true;
            }
        }
    }
    
    return false;
}

float ObstacleDetector::get_nearest_obstacle(Vector2 point, float max_radius) const {
    if (!tilemap_) return max_radius;
    
    // Convert point to tile coordinates
    Vector2 tile_pos = tilemap_->world_to_tile(point.x, point.y);
    int centerTileX = static_cast<int>(tile_pos.x);
    int centerTileY = static_cast<int>(tile_pos.y);
    
    // Calculate search radius in tiles
    int tileRadius = static_cast<int>(std::ceil(max_radius / tilemap_->get_tile_size()));
    
    float min_distance = max_radius;
    
    // Check all tiles in search radius
    for (int y = centerTileY - tileRadius; y <= centerTileY + tileRadius; y++) {
        for (int x = centerTileX - tileRadius; x <= centerTileX + tileRadius; x++) {
            // Skip if out of bounds
            if (x < 0 || x >= tilemap_->get_width() || y < 0 || y >= tilemap_->get_height())
                continue;
            
            if (!tilemap_->is_walkable(x, y)) {
                // Calculate tile center
                Vector2 tile_center = tilemap_->tile_to_world(x, y);
                tile_center.x += tilemap_->get_tile_size() / 2.0f;
                tile_center.y += tilemap_->get_tile_size() / 2.0f;
                
                // Calculate distance to tile (simplified to distance to center)
                float dist = shared::math::distance(point, tile_center);
                
                // Account for tile size (approximate edge distance)
                dist -= tilemap_->get_tile_size() / 2.0f;
                
                // Update minimum distance
                min_distance = std::min(min_distance, dist);
            }
        }
    }
    
    return min_distance;
}

void ObstacleDetector::create_steering_grid(Vector2 center, float* out_distances, int num_rays, float max_distance) const {
    if (!tilemap_ || !out_distances) return;
    
    // Calculate rays in all directions
    const float angle_step = 2.0f * PI / num_rays;
    
    for (int i = 0; i < num_rays; i++) {
        float angle = i * angle_step;
        Vector2 direction = {
            std::cos(angle),
            std::sin(angle)
        };
        
        // Cast ray and store distance
        RaycastHit hit = raycast(center, direction, max_distance);
        out_distances[i] = hit.hit ? hit.distance : max_distance;
    }
}

void ObstacleDetector::draw_debug(bool screen_space) const {
    if (!tilemap_ || !show_debug_) return;
    
    // Draw obstacle detection visualization
    int width = tilemap_->get_width();
    int height = tilemap_->get_height();
    int tile_size = tilemap_->get_tile_size();
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (!tilemap_->is_walkable(x, y)) {
                // Calculate rectangle for obstacle
                Rectangle rect = {
                    static_cast<float>(x * tile_size),
                    static_cast<float>(y * tile_size),
                    static_cast<float>(tile_size),
                    static_cast<float>(tile_size)
                };
                
                // Convert to screen space if needed
                if (screen_space) {
                    // Use an alternative method to convert world space to screen space
                    // since world::world_to_screen isn't directly accessible here
                    
                    // Option 1: Skip the conversion if not needed for debugging
                    // We'll just use world coordinates as-is
                    
                    // Option 2: Comment out for now until proper implementation
                    // Vector2 topLeft = world::world_to_screen({rect.x, rect.y});
                    // rect.x = topLeft.x;
                    // rect.y = topLeft.y;
                    
                    // For now, just use world coordinates directly
                }
                
                // Draw outline of obstacle
                DrawRectangleLinesEx(rect, 1.0f, RED);
            }
        }
    }
}

bool ObstacleDetector::is_obstacle(int tile_x, int tile_y) const {
    if (!tilemap_) return false;
    
    // Check if tile is within bounds
    if (tile_x < 0 || tile_x >= tilemap_->get_width() || 
        tile_y < 0 || tile_y >= tilemap_->get_height())
        return true; // Out of bounds is considered an obstacle
    
    // Check if tile is walkable
    return !tilemap_->is_walkable(tile_x, tile_y);
}

float ObstacleDetector::distance_to_obstacle(Vector2 origin, Vector2 direction, float max_distance) const {
    RaycastHit hit = raycast(origin, direction, max_distance);
    return hit.hit ? hit.distance : max_distance;
}

} // namespace atoms
} // namespace world 