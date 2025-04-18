/// player.cpp — organism for Player slice
#include "player.hpp"
#include "molecules/controller.hpp"
#include <memory> // for std::unique_ptr

namespace player {

// Player singleton instance (private implementation)
namespace {
    std::unique_ptr<molecules::PlayerController> controller;
}

void init() {
    // Create controller
    controller = std::make_unique<molecules::PlayerController>();
    
    // Initialize at center of screen
    controller->init(
        static_cast<float>(GetScreenWidth()) / 2,
        static_cast<float>(GetScreenHeight()) / 2
    );
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

}  // namespace player 