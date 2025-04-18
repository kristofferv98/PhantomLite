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
int CollisionWorld::add_object(const CollisionObject& object) {
    CollisionObject new_object = object;
    new_object.id = next_id_++;
    objects_.push_back(new_object);
    return new_object.id;
}

void CollisionWorld::update_object_position(int id, Vector2 position) {
    for (auto& object : objects_) {
        if (object.id == id) {
            object.position = position;
            return;
        }
    }
}

void CollisionWorld::remove_object(int id) {
    objects_.erase(
        std::remove_if(objects_.begin(), objects_.end(),
            [id](const CollisionObject& obj) { return obj.id == id; }),
        objects_.end()
    );
}

const std::vector<CollisionObject>& CollisionWorld::get_objects() const {
    return objects_;
}

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
    
    // Test against all other objects
    for (const auto& other : objects_) {
        // Skip self
        if (other.id == object_id) {
            continue;
        }
        
        // Only test against solid objects
        if (!other.is_solid) {
            continue;
        }
        
        Vector2 penetration = {0, 0};
        if (check_collision(test_copy, other, &penetration)) {
            result.collided = true;
            result.penetration = penetration;
            result.object_id = other.id;
            return result; // Return on first collision found
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
        // Find penetration depths on each axis
        float x_overlap_right = a_right - b_left;
        float x_overlap_left = b_right - a_left;
        float y_overlap_bottom = a_bottom - b_top;
        float y_overlap_top = b_bottom - a_top;
        
        // Find minimum penetration vector
        float x_penetration = (x_overlap_right < x_overlap_left) ? -x_overlap_right : x_overlap_left;
        float y_penetration = (y_overlap_bottom < y_overlap_top) ? -y_overlap_bottom : y_overlap_top;
        
        // Use the axis with the smaller penetration
        if (std::abs(x_penetration) < std::abs(y_penetration)) {
            penetration->x = x_penetration;
            penetration->y = 0;
        } else {
            penetration->x = 0;
            penetration->y = y_penetration;
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