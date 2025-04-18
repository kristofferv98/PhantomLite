add_library(core_interface
    entity_adapter.cpp
    world_adapter.cpp
    ui_adapter.cpp
)

target_include_directories(core_interface PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(core_interface
    player_feature
    world_feature
    ui_feature
    raylib
) 