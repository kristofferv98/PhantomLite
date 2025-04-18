/// collision.cpp — implementation of collision detection atom
#include "collision.hpp"
#include <algorithm>
#include <cmath>

namespace player {
namespace atoms {

// Factory functions for collision shapes
CollisionShape CollisionShape::Rectangle(float width, float height, Vector2 offset) {
    CollisionShape shape;
    shape.type = CollisionShapeType::RECTANGLE;
    shape.offset = offset;
    shape.rect.width = width;
    shape.rect.height = height;
    return shape;
}

CollisionShape CollisionShape::Circle(float radius, Vector2 offset) {
    CollisionShape shape;
    shape.type = CollisionShapeType::CIRCLE;
    shape.offset = offset;
    shape.circle.radius = radius;
    return shape;
}

// CollisionWorld implementation
CollisionWorld::CollisionWorld(float cell_size, int max_objects_per_cell)
    : cell_size_(cell_size), max_objects_per_cell_(max_objects_per_cell) {
    // Initialize spatial grid with a default size
    // It will grow as needed based on world bounds
    grid_width_ = 20;
    grid_height_ = 20;
    grid_.resize(grid_width_ * grid_height_);
}

int CollisionWorld::get_cell_index(float x, float y) const {
    int cell_x = static_cast<int>(x / cell_size_);
    int cell_y = static_cast<int>(y / cell_size_);
    
    // Clamp to grid bounds
    cell_x = std::max(0, std::min(cell_x, grid_width_ - 1));
    cell_y = std::max(0, std::min(cell_y, grid_height_ - 1));
    
    return cell_y * grid_width_ + cell_x;
}

std::vector<int> CollisionWorld::get_neighboring_cells(const CollisionObject& object) const {
    std::vector<int> cells;
    Vector2 pos = object.position;
    float radius = 0.0f;
    
    // Calculate bounds based on shape type
    float min_x, min_y, max_x, max_y;
    if (object.shape.type == CollisionShapeType::RECTANGLE) {
        min_x = pos.x + object.shape.offset.x - object.shape.rect.width / 2;
        max_x = pos.x + object.shape.offset.x + object.shape.rect.width / 2;
        min_y = pos.y + object.shape.offset.y - object.shape.rect.height / 2;
        max_y = pos.y + object.shape.offset.y + object.shape.rect.height / 2;
    } else { // Circle
        radius = object.shape.circle.radius;
        min_x = pos.x + object.shape.offset.x - radius;
        max_x = pos.x + object.shape.offset.x + radius;
        min_y = pos.y + object.shape.offset.y - radius;
        max_y = pos.y + object.shape.offset.y + radius;
    }
    
    // Get cell indices for min and max bounds
    int min_cell_x = static_cast<int>(min_x / cell_size_);
    int min_cell_y = static_cast<int>(min_y / cell_size_);
    int max_cell_x = static_cast<int>(max_x / cell_size_);
    int max_cell_y = static_cast<int>(max_y / cell_size_);
    
    // Clamp to grid bounds
    min_cell_x = std::max(0, std::min(min_cell_x, grid_width_ - 1));
    min_cell_y = std::max(0, std::min(min_cell_y, grid_height_ - 1));
    max_cell_x = std::max(0, std::min(max_cell_x, grid_width_ - 1));
    max_cell_y = std::max(0, std::min(max_cell_y, grid_height_ - 1));
    
    // Add all cells in the range
    for (int y = min_cell_y; y <= max_cell_y; y++) {
        for (int x = min_cell_x; x <= max_cell_x; x++) {
            cells.push_back(y * grid_width_ + x);
        }
    }
    
    return cells;
}

void CollisionWorld::update_object_in_grid(int id) {
    // First remove from all cells
    for (auto& cell : grid_) {
        cell.object_ids.erase(
            std::remove(cell.object_ids.begin(), cell.object_ids.end(), id),
            cell.object_ids.end()
        );
    }
    
    // Find the object
    const CollisionObject* obj = nullptr;
    for (const auto& object : objects_) {
        if (object.id == id) {
            obj = &object;
            break;
        }
    }
    
    if (!obj) return; // Object not found
    
    // Add to new cells
    auto cells = get_neighboring_cells(*obj);
    for (int cell_idx : cells) {
        if (cell_idx >= 0 && cell_idx < grid_.size()) {
            grid_[cell_idx].object_ids.push_back(id);
        }
    }
}

// Update object position implementation
void CollisionWorld::update_object_position(int id, Vector2 position) {
    for (auto& object : objects_) {
        if (object.id == id) {
            object.position = position;
            update_object_in_grid(id);
            return;
        }
    }
}

// Add object implementation
int CollisionWorld::add_object(const CollisionObject& object) {
    CollisionObject new_object = object;
    new_object.id = next_id_++;
    objects_.push_back(new_object);
    
    // Add to spatial grid
    update_object_in_grid(new_object.id);
    
    return new_object.id;
}

// Remove object implementation
void CollisionWorld::remove_object(int id) {
    // Remove from grid first
    for (auto& cell : grid_) {
        cell.object_ids.erase(
            std::remove(cell.object_ids.begin(), cell.object_ids.end(), id),
            cell.object_ids.end()
        );
    }
    
    // Then remove from objects list
    objects_.erase(
        std::remove_if(objects_.begin(), objects_.end(),
            [id](const CollisionObject& obj) { return obj.id == id; }),
        objects_.end()
    );
}

// Get all objects in the world
const std::vector<CollisionObject>& CollisionWorld::get_objects() const {
    return objects_;
}

// Get object by ID
const CollisionObject* CollisionWorld::get_object(int id) const {
    for (const auto& obj : objects_) {
        if (obj.id == id) {
            return &obj;
        }
    }
    return nullptr;
}

// Draw debug visualization of the spatial grid
void CollisionWorld::draw_debug_grid() const {
    // Draw grid cells
    for (int y = 0; y < grid_height_; y++) {
        for (int x = 0; x < grid_width_; x++) {
            int cell_idx = y * grid_width_ + x;
            
            // Calculate cell rectangle
            Rectangle cell_rect = {
                x * cell_size_,
                y * cell_size_,
                cell_size_,
                cell_size_
            };
            
            // Color based on object count
            int object_count = grid_[cell_idx].object_ids.size();
            Color cell_color = GRAY;
            cell_color.a = (object_count > 0) ? 50 + std::min(object_count * 20, 205) : 20;
            
            DrawRectangleLinesEx(cell_rect, 1.0f, cell_color);
            
            // Draw object count in cell if non-zero
            if (object_count > 0) {
                char count_text[8];
                sprintf(count_text, "%d", object_count);
                DrawText(count_text, 
                         static_cast<int>(cell_rect.x + cell_rect.width/2 - 5), 
                         static_cast<int>(cell_rect.y + cell_rect.height/2 - 10),
                         20, WHITE);
            }
        }
    }
}

// Optimized collision testing with spatial partitioning
CollisionResult CollisionWorld::test_collision(int object_id, Vector2 new_position) {
    CollisionResult result = { false, {0, 0}, -1 };
    
    // Find the object to test
    CollisionObject* test_object = nullptr;
    for (auto& obj : objects_) {
        if (obj.id == object_id) {
            test_object = &obj;
            break;
        }
    }
    
    if (!test_object) {
        return result; // Object not found
    }
    
    // Create a copy with the new position for testing
    CollisionObject test_copy = *test_object;
    test_copy.position = new_position;
    
    // Get potential collision cells
    std::vector<int> cells = get_neighboring_cells(test_copy);
    
    // Store IDs we've already checked to avoid duplicates
    std::vector<int> checked_ids;
    
    // Test against objects in neighboring cells
    for (int cell_idx : cells) {
        if (cell_idx < 0 || cell_idx >= grid_.size()) continue;
        
        for (int other_id : grid_[cell_idx].object_ids) {
            // Skip self
            if (other_id == object_id) continue;
            
            // Skip if already checked
            if (std::find(checked_ids.begin(), checked_ids.end(), other_id) != checked_ids.end()) {
                continue;
            }
            
            // Find the other object
            const CollisionObject* other = nullptr;
            for (const auto& obj : objects_) {
                if (obj.id == other_id) {
                    other = &obj;
                    break;
                }
            }
            
            if (!other || !other->is_solid) continue;
            
            // Mark as checked
            checked_ids.push_back(other_id);
            
            // Check for collision
            Vector2 penetration = {0, 0};
            if (check_collision(test_copy, *other, &penetration)) {
                result.collided = true;
                result.penetration = penetration;
                result.object_id = other_id;
                return result; // Return on first collision
            }
        }
    }
    
    return result;
}

bool CollisionWorld::check_collision(const CollisionObject& a, const CollisionObject& b, Vector2* penetration) {
    // Dispatch to appropriate collision check function based on shape types
    if (a.shape.type == CollisionShapeType::RECTANGLE && b.shape.type == CollisionShapeType::RECTANGLE) {
        return check_rect_rect(a, b, penetration);
    }
    else if (a.shape.type == CollisionShapeType::CIRCLE && b.shape.type == CollisionShapeType::CIRCLE) {
        return check_circle_circle(a, b, penetration);
    }
    else if (a.shape.type == CollisionShapeType::RECTANGLE && b.shape.type == CollisionShapeType::CIRCLE) {
        return check_rect_circle(a, b, penetration);
    }
    else if (a.shape.type == CollisionShapeType::CIRCLE && b.shape.type == CollisionShapeType::RECTANGLE) {
        // Swap arguments and negate penetration
        bool result = check_rect_circle(b, a, penetration);
        if (result && penetration) {
            penetration->x = -penetration->x;
            penetration->y = -penetration->y;
        }
        return result;
    }
    
    return false;
}

bool CollisionWorld::check_rect_rect(const CollisionObject& a, const CollisionObject& b, Vector2* penetration) {
    // Calculate rectangle bounds with offset
    float a_left = a.position.x + a.shape.offset.x - a.shape.rect.width / 2;
    float a_right = a.position.x + a.shape.offset.x + a.shape.rect.width / 2;
    float a_top = a.position.y + a.shape.offset.y - a.shape.rect.height / 2;
    float a_bottom = a.position.y + a.shape.offset.y + a.shape.rect.height / 2;
    
    float b_left = b.position.x + b.shape.offset.x - b.shape.rect.width / 2;
    float b_right = b.position.x + b.shape.offset.x + b.shape.rect.width / 2;
    float b_top = b.position.y + b.shape.offset.y - b.shape.rect.height / 2;
    float b_bottom = b.position.y + b.shape.offset.y + b.shape.rect.height / 2;
    
    // Check for overlap
    if (a_right <= b_left || a_left >= b_right || a_bottom <= b_top || a_top >= b_bottom) {
        return false; // No collision
    }
    
    // Calculate penetration if requested
    if (penetration) {
        // Find penetration in both axes
        float x_overlap = std::min(a_right - b_left, b_right - a_left);
        float y_overlap = std::min(a_bottom - b_top, b_bottom - a_top);
        
        // Debug collision info
        #ifdef DEBUG_COLLISIONS
        TraceLog(LOG_DEBUG, "Collision: x_overlap=%.2f, y_overlap=%.2f", x_overlap, y_overlap);
        #endif
        
        // Use the axis with the smaller penetration
        if (x_overlap < y_overlap) {
            // X-axis has smaller penetration - determine direction based on centers
            penetration->x = (a.position.x < b.position.x) ? -x_overlap : x_overlap;
            penetration->y = 0;
            
            #ifdef DEBUG_COLLISIONS
            TraceLog(LOG_DEBUG, "X-axis resolution: penetration=(%.2f, %.2f)", 
                    penetration->x, penetration->y);
            #endif
        } else {
            // Y-axis has smaller penetration - determine direction based on centers
            penetration->x = 0;
            penetration->y = (a.position.y < b.position.y) ? -y_overlap : y_overlap;
            
            #ifdef DEBUG_COLLISIONS
            TraceLog(LOG_DEBUG, "Y-axis resolution: penetration=(%.2f, %.2f)", 
                    penetration->x, penetration->y);
            #endif
        }
    }
    
    return true;
}

bool CollisionWorld::check_circle_circle(const CollisionObject& a, const CollisionObject& b, Vector2* penetration) {
    // Calculate actual positions with offset
    Vector2 pos_a = {
        a.position.x + a.shape.offset.x,
        a.position.y + a.shape.offset.y
    };
    
    Vector2 pos_b = {
        b.position.x + b.shape.offset.x,
        b.position.y + b.shape.offset.y
    };
    
    // Calculate distance between centers
    float dx = pos_b.x - pos_a.x;
    float dy = pos_b.y - pos_a.y;
    float distance_squared = dx * dx + dy * dy;
    
    // Sum of radii
    float radii_sum = a.shape.circle.radius + b.shape.circle.radius;
    
    // Check if circles collide
    if (distance_squared >= radii_sum * radii_sum) {
        return false; // No collision
    }
    
    // Calculate penetration if requested
    if (penetration) {
        float distance = std::sqrt(distance_squared);
        
        // Normalize direction vector
        if (distance > 0) {
            penetration->x = dx / distance * (radii_sum - distance);
            penetration->y = dy / distance * (radii_sum - distance);
        } else {
            // If circles are exactly at same position, push along x-axis
            penetration->x = radii_sum;
            penetration->y = 0;
        }
    }
    
    return true;
}

bool CollisionWorld::check_rect_circle(const CollisionObject& rect, const CollisionObject& circle, Vector2* penetration) {
    // Calculate actual positions with offset
    Vector2 rect_pos = {
        rect.position.x + rect.shape.offset.x,
        rect.position.y + rect.shape.offset.y
    };
    
    Vector2 circle_pos = {
        circle.position.x + circle.shape.offset.x,
        circle.position.y + circle.shape.offset.y
    };
    
    // Rectangle extents
    float rect_half_width = rect.shape.rect.width / 2;
    float rect_half_height = rect.shape.rect.height / 2;
    
    // Find closest point on rectangle to circle center
    float closest_x = std::clamp(circle_pos.x, rect_pos.x - rect_half_width, rect_pos.x + rect_half_width);
    float closest_y = std::clamp(circle_pos.y, rect_pos.y - rect_half_height, rect_pos.y + rect_half_height);
    
    // Calculate distance from closest point to circle center
    float dx = circle_pos.x - closest_x;
    float dy = circle_pos.y - closest_y;
    float distance_squared = dx * dx + dy * dy;
    
    // Check if distance is less than circle radius
    if (distance_squared > circle.shape.circle.radius * circle.shape.circle.radius) {
        return false; // No collision
    }
    
    // Calculate penetration if requested
    if (penetration) {
        if (distance_squared > 0) {
            float distance = std::sqrt(distance_squared);
            penetration->x = dx / distance * (circle.shape.circle.radius - distance);
            penetration->y = dy / distance * (circle.shape.circle.radius - distance);
        } else {
            // If circle center is inside rectangle, push out in nearest direction
            float dx_left = circle_pos.x - (rect_pos.x - rect_half_width);
            float dx_right = (rect_pos.x + rect_half_width) - circle_pos.x;
            float dy_top = circle_pos.y - (rect_pos.y - rect_half_height);
            float dy_bottom = (rect_pos.y + rect_half_height) - circle_pos.y;
            
            // Find smallest penetration
            float min_dist = std::min({dx_left, dx_right, dy_top, dy_bottom});
            
            if (min_dist == dx_left) {
                penetration->x = -dx_left - circle.shape.circle.radius;
                penetration->y = 0;
            } else if (min_dist == dx_right) {
                penetration->x = dx_right + circle.shape.circle.radius;
                penetration->y = 0;
            } else if (min_dist == dy_top) {
                penetration->x = 0;
                penetration->y = -dy_top - circle.shape.circle.radius;
            } else {
                penetration->x = 0;
                penetration->y = dy_bottom + circle.shape.circle.radius;
            }
        }
    }
    
    return true;
}

} // namespace atoms
} // namespace player 