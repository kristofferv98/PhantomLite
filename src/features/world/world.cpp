/// world.cpp — organism for World slice
#include "world.hpp"
#include "atoms/tilemap.hpp"
#include "atoms/camera.hpp"
#include <memory>

namespace world {

namespace {
    // Private implementation details
    std::unique_ptr<atoms::Tilemap> tilemap;
    std::unique_ptr<atoms::Camera> camera;
    
    // Map configuration
    constexpr int MAP_WIDTH = 50;
    constexpr int MAP_HEIGHT = 50;
    constexpr int TILE_SIZE = 32;
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
}

void update(float dt) {
    if (camera) {
        camera->update(dt);
    }
}

void render() {
    if (tilemap && camera) {
        tilemap->render(camera->get_view());
    }
}

void cleanup() {
    if (tilemap) {
        tilemap->cleanup();
        tilemap.reset();
    }
    
    camera.reset();
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

}  // namespace world 