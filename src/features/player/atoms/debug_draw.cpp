/// debug_draw.cpp — implementation of debug visualization atom
#include "debug_draw.hpp"

namespace player {
namespace atoms {

void draw_collision_shape(const CollisionShape& shape, Vector2 position, Color color) {
    // Calculate actual position with offset
    Vector2 pos = {
        position.x + shape.offset.x,
        position.y + shape.offset.y
    };
    
    // Draw based on shape type
    switch (shape.type) {
        case CollisionShapeType::RECTANGLE:
            DrawRectangleLines(
                static_cast<int>(pos.x - shape.rect.width / 2),
                static_cast<int>(pos.y - shape.rect.height / 2),
                static_cast<int>(shape.rect.width),
                static_cast<int>(shape.rect.height),
                color
            );
            break;
            
        case CollisionShapeType::CIRCLE:
            DrawCircleLines(
                static_cast<int>(pos.x),
                static_cast<int>(pos.y),
                shape.circle.radius,
                color
            );
            break;
    }
}

void draw_collision_world(const CollisionWorld& world, Color color) {
    for (const auto& object : world.get_objects()) {
        // Use different colors for solid vs. non-solid objects
        Color obj_color = object.is_solid ? color : GRAY;
        draw_collision_shape(object.shape, object.position, obj_color);
    }
}

} // namespace atoms
} // namespace player 