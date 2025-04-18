file(GLOB_RECURSE SRC_PLAYER *.cpp)
add_library(player_feature STATIC
    player.cpp
    atoms/movement.cpp
    atoms/animation.cpp
    atoms/actions.cpp
    atoms/collision.cpp
    atoms/health.cpp
    atoms/debug_draw.cpp
    molecules/controller.cpp
    molecules/hearts_controller.cpp
)
target_link_libraries(player_feature PUBLIC core)
target_include_directories(player_feature PUBLIC ${CMAKE_CURRENT_LIST_DIR}) 