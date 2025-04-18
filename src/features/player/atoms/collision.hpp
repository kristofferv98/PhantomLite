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
    // Constructor with optional grid settings
    CollisionWorld(float cell_size = 128.0f, int max_objects_per_cell = 10);
    
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
    
    // Get collision object by ID
    const CollisionObject* get_object(int id) const;
    
    // Debug visualization
    void draw_debug_grid() const;
    
private:
    struct SpatialCell {
        std::vector<int> object_ids;
    };
    
    std::vector<CollisionObject> objects_;
    int next_id_ = 0;
    
    // Spatial partitioning grid
    std::vector<SpatialCell> grid_;
    float cell_size_;
    int grid_width_ = 0;
    int grid_height_ = 0;
    int max_objects_per_cell_;
    
    // Get cell index from world position
    int get_cell_index(float x, float y) const;
    
    // Get neighboring cells for an object
    std::vector<int> get_neighboring_cells(const CollisionObject& object) const;
    
    // Update object's position in the spatial grid
    void update_object_in_grid(int id);
    
    // Collision detection helpers
    bool check_collision(const CollisionObject& a, const CollisionObject& b, Vector2* penetration = nullptr);
    bool check_rect_rect(const CollisionObject& a, const CollisionObject& b, Vector2* penetration);
    bool check_circle_circle(const CollisionObject& a, const CollisionObject& b, Vector2* penetration);
    bool check_rect_circle(const CollisionObject& rect, const CollisionObject& circle, Vector2* penetration);
};

} // namespace atoms
} // namespace player 