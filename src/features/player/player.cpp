/// player.cpp — organism for Player slice
#include "player.hpp"
#include "molecules/controller.hpp"
#include "../ui/ui.hpp"  // Use the proper UI API
#include <memory> // for std::unique_ptr
#include "atoms/collision.hpp"

namespace player {

// Global player controller instance
static std::unique_ptr<molecules::PlayerController> controller;

void init(float start_x, float start_y) {
    // Create and initialize the controller
    controller = std::make_unique<molecules::PlayerController>();
    controller->init(start_x, start_y);
}

void update(float dt) {
    if (controller) {
        controller->update(dt);
    }
}

void render() {
    if (controller) {
        controller->render();
    }
}

void cleanup() {
    if (controller) {
        controller->cleanup();
        controller.reset();
    }
}

void set_animation(PlayerState state) {
    // Deprecated - animation state is now managed by the controller
}

Vector2 get_position() {
    if (controller) {
        return controller->get_position();
    }
    // Return center of screen as fallback
    return {
        static_cast<float>(GetScreenWidth()) / 2,
        static_cast<float>(GetScreenHeight()) / 2
    };
}

bool take_damage(int pips, Vector2 knockback_dir) {
    // Forward to controller if it exists
    if (controller) {
        return controller->take_damage(pips, knockback_dir);
    }
    return false;
}

int get_health() {
    if (controller) {
        return controller->get_health();
    }
    return 0;
}

}  // namespace player 