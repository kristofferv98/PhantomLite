# UI Slice

The UI slice handles all game user interface elements including health displays, menus, HUD, and dialog systems.

## Context Primer

This slice manages the rendering and interaction of UI elements that overlay the game world. It includes:

- Health/hearts display system
- Main menu and pause menu implementation
- In-game HUD with player stats
- Dialog boxes for NPC interactions
- UI sound effects and feedback

The UI elements use assets from `assets/ui/` and operate on a separate render layer above the game world.

## Structure
- **atoms/**: Core UI elements (hearts, menu items, dialog, notifications)
- **molecules/**: Controllers for UI elements (hearts_controller, menu_controller)
- **ui.hpp/cpp**: Public API (organism)

## Usage Example
```cpp
// Initialize and use UI
ui::init();
ui::update(delta_time);
ui::render();
ui::set_player_health(current, max);
ui::show_dialog("Character", "Hello world!");
```

## Key Concepts
- UI uses screen-space coordinates (0,0 at top-left)
- Rendering layers: background, content, HUD, notification
- Input capture for UI navigation
- Screen-resolution independent

## UI Components
- Hearts: Player health visualization
- Menus: Game options and inventory
- Dialogs: Character conversations
- Notifications: Temporary alerts