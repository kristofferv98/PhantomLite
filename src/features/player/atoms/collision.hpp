/// collision.hpp — collision detection atom for player slice
#pragma once
#include <raylib.h>
#include <vector>

namespace player {
namespace atoms {

// Collision shape type
enum class CollisionShapeType {
    RECTANGLE,
    CIRCLE
};

// Unified collision shape structure
struct CollisionShape {
    CollisionShapeType type;
    Vector2 offset;      // Offset from entity position
    
    // Shape-specific data
    union {
        struct {
            float width;
            float height;
        } rect;
        
        struct {
            float radius;
        } circle;
    };
    
    // Factory functions for creating shapes
    static CollisionShape Rectangle(float width, float height, Vector2 offset = {0, 0});
    static CollisionShape Circle(float radius, Vector2 offset = {0, 0});
};

// Collision object represents any entity with position and collision shape
struct CollisionObject {
    Vector2 position;
    CollisionShape shape;
    bool is_solid;
    int id;  // Optional identifier
};

// Collision result stores information about a collision
struct CollisionResult {
    bool collided;
    Vector2 penetration;  // How much objects are overlapping
    int object_id;        // ID of the object collided with
};

// Create a collision world to manage and test collision objects
class CollisionWorld {
public:
    // Add an object to the collision world
    int add_object(const CollisionObject& object);
    
    // Update an object's position
    void update_object_position(int id, Vector2 position);
    
    // Remove an object from the world
    void remove_object(int id);
    
    // Test if an object would collide at a new position
    CollisionResult test_collision(int object_id, Vector2 new_position);
    
    // Get all objects in the world
    const std::vector<CollisionObject>& get_objects() const;
    
private:
    std::vector<CollisionObject> objects_;
    int next_id_ = 0;
    
    // Collision detection helpers
    bool check_collision(const CollisionObject& a, const CollisionObject& b, Vector2* penetration = nullptr);
    bool check_rect_rect(const CollisionObject& a, const CollisionObject& b, Vector2* penetration);
    bool check_circle_circle(const CollisionObject& a, const CollisionObject& b, Vector2* penetration);
    bool check_rect_circle(const CollisionObject& rect, const CollisionObject& circle, Vector2* penetration);
};

} // namespace atoms
} // namespace player 