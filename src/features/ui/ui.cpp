/// ui.cpp — Implementation of UI slice public API

#include "ui.hpp"
#include "molecules/hearts_controller.hpp"

namespace ui {

void init_ui() {
    HeartsController::init();
}

void update_ui(float dt) {
    HeartsController::update(dt);
}

void render_ui() {
    HeartsController::render();
}

void cleanup_ui() {
    HeartsController::cleanup();
}

} // namespace ui 