/// world.cpp — organism for World slice
#include "world.hpp"
#include "atoms/tilemap.hpp"
#include "atoms/camera.hpp"
#include "atoms/obstacle_detector.hpp"
#include <memory>

namespace world {

namespace {
    // Private implementation details
    std::unique_ptr<atoms::Tilemap> tilemap;
    std::unique_ptr<atoms::Camera> camera;
    std::unique_ptr<atoms::ObstacleDetector> obstacle_detector;
    
    // Map configuration
    constexpr int MAP_WIDTH = 50;
    constexpr int MAP_HEIGHT = 50;
    constexpr int TILE_SIZE = 32;
    
    // Debug flags
    bool show_obstacle_debug = false;
}

void init() {
    // Initialize tilemap
    tilemap = std::make_unique<atoms::Tilemap>();
    tilemap->init(MAP_WIDTH, MAP_HEIGHT, TILE_SIZE);
    tilemap->load_textures();
    tilemap->generate_demo_map();
    
    // Initialize camera
    camera = std::make_unique<atoms::Camera>();
    camera->init(GetScreenWidth(), GetScreenHeight());
    
    // Set camera bounds to match world size
    camera->set_bounds(
        0, 0,
        MAP_WIDTH * TILE_SIZE,
        MAP_HEIGHT * TILE_SIZE
    );
    
    // Initialize obstacle detector
    obstacle_detector = std::make_unique<atoms::ObstacleDetector>();
    obstacle_detector->init(tilemap.get());
}

void update(float dt) {
    if (camera) {
        camera->update(dt);
    }
}

void render() {
    if (tilemap && camera) {
        tilemap->render(camera->get_view());
        
        // Draw obstacle debug visualization if enabled
        if (show_obstacle_debug && obstacle_detector) {
            obstacle_detector->draw_debug(true);
        }
    }
}

void cleanup() {
    if (tilemap) {
        tilemap->cleanup();
        tilemap.reset();
    }
    
    camera.reset();
    obstacle_detector.reset();
}

void set_camera_target(const Vector2& target) {
    if (camera) {
        camera->set_target(target);
    }
}

bool is_walkable(float world_x, float world_y) {
    if (tilemap) {
        return tilemap->is_walkable(world_x, world_y);
    }
    return true; // Default if no tilemap
}

void get_world_bounds(float* out_min_x, float* out_min_y, float* out_max_x, float* out_max_y) {
    if (tilemap) {
        if (out_min_x) *out_min_x = 0;
        if (out_min_y) *out_min_y = 0;
        if (out_max_x) *out_max_x = tilemap->get_width() * tilemap->get_tile_size();
        if (out_max_y) *out_max_y = tilemap->get_height() * tilemap->get_tile_size();
    }
}

Vector2 screen_to_world(const Vector2& screen_pos) {
    if (camera) {
        return camera->screen_to_world(screen_pos);
    }
    return screen_pos; // Default if no camera
}

Vector2 world_to_screen(const Vector2& world_pos) {
    if (camera) {
        return camera->world_to_screen(world_pos);
    }
    return world_pos; // Default if no camera
}

// NEW: Obstacle detection functions implementation

float raycast(Vector2 origin, Vector2 direction, float max_distance) {
    if (obstacle_detector) {
        atoms::RaycastHit hit = obstacle_detector->raycast(origin, direction, max_distance);
        return hit.hit ? hit.distance : max_distance;
    }
    return max_distance;
}

void get_steering_distances(Vector2 position, float* out_distances, int num_rays, float max_distance) {
    if (obstacle_detector && out_distances) {
        obstacle_detector->create_steering_grid(position, out_distances, num_rays, max_distance);
    }
}

bool check_circle_collision(Vector2 center, float radius) {
    if (obstacle_detector) {
        return obstacle_detector->check_circle_overlap(center, radius);
    }
    return false;
}

bool check_rect_collision(Rectangle rect) {
    if (obstacle_detector) {
        return obstacle_detector->check_rect_overlap(rect);
    }
    return false;
}

void toggle_obstacle_debug() {
    show_obstacle_debug = !show_obstacle_debug;
}

}  // namespace world 