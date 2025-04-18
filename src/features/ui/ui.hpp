/// ui.hpp — public API for the UI slice (health, inventory, dialog)

#pragma once

namespace ui {
  void init_ui();
  void update_ui(float dt);
  void render_ui();
  void cleanup_ui();
} 