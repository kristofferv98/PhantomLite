#include "enemy_movement.hpp"
#include "../enemy_slime.hpp"
#include "enemy_state.hpp"
#include "core/public/entity.hpp"
#include "../../world/world.hpp"
#include "shared/math_utils.hpp"
#include <vector>
#include <cmath>

namespace enemy {
namespace atoms {

static world::atoms::Tilemap* tilemap = nullptr;
static const float PATH_UPDATE_INTERVAL = 0.5f; // seconds
static float path_timer = 0.0f;

void init_movement(world::atoms::Tilemap* map) {
    tilemap = map;
    path_timer = 0.0f;
}

void update_movement(Vector2 player_position, float delta_time) {
    // Get enemies from our own state management as non-const to allow modifications
    auto& enemies = get_enemies_mutable();
    
    path_timer += delta_time;
    bool should_recalculate_paths = path_timer >= PATH_UPDATE_INTERVAL;
    
    if (should_recalculate_paths) {
        path_timer = 0.0f;
    }
    
    for (auto& enemy : enemies) {
        if (!enemy.active) continue;
        
        // Simple direct movement toward player with obstacle avoidance
        Vector2 direction = {
            player_position.x - enemy.position.x,
            player_position.y - enemy.position.y
        };
        
        float distance = sqrt(direction.x * direction.x + direction.y * direction.y);
        
        // Only move if not too close to player
        if (distance > enemy.spec->radius * 1.5f) {
            // Normalize direction
            if (distance > 0) {
                direction.x /= distance;
                direction.y /= distance;
            }
            
            // Apply movement speed
            float speed = enemy.spec->speed;
            
            Vector2 potential_position = {
                enemy.position.x + direction.x * speed * delta_time,
                enemy.position.y + direction.y * speed * delta_time
            };
            
            // Simple collision check with world
            bool can_move = world::is_walkable(potential_position.x, potential_position.y);
            
            if (can_move) {
                enemy.position.x += direction.x * speed * delta_time;
                enemy.position.y += direction.y * speed * delta_time;
                
                // Update collision rectangle
                enemy.collision_rect.x = enemy.position.x - enemy.spec->size.x/2;
                enemy.collision_rect.y = enemy.position.y - enemy.spec->size.y/2;
                
                // Update facing direction based on movement
                if (fabs(direction.x) > fabs(direction.y)) {
                    enemy.facing = (direction.x > 0) ? enemies::Facing::RIGHT : enemies::Facing::LEFT;
                } else {
                    enemy.facing = (direction.y > 0) ? enemies::Facing::DOWN : enemies::Facing::UP;
                }
                
                // Mark as moving
                enemy.is_moving = true;
            }
        }
    }
}

void cleanup_movement() {
    tilemap = nullptr;
}

} // namespace atoms
} // namespace enemy 