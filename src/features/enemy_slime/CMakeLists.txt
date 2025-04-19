# Enemy_slime Slice CMakeLists.txt

# Explicitly list implementation files instead of globbing
set(SRC_ENEMY 
    atoms/behavior_atoms.cpp
    atoms/enemy_state.cpp
    atoms/enemy_spawner.cpp
    atoms/enemy_spawning.cpp
    atoms/enemy_renderer.cpp
    enemy_slime.cpp
)

# Create the enemy_slime library target
add_library(enemy_slime STATIC ${SRC_ENEMY})
target_link_libraries(enemy_slime 
    PUBLIC 
    core 
    player_feature 
    world_feature 
    shared_utils
    enemies_feature  # Add dependency on common enemies library
)
target_include_directories(enemy_slime PUBLIC ${CMAKE_CURRENT_LIST_DIR})

# Skip tests for now - they're not properly set up
# We'll add this back later with proper Catch2 setup

# Add tests
add_executable(test_slime_spec
    tests/test_slime_spec.cpp
)

target_link_libraries(test_slime_spec
    PRIVATE enemy_slime
    PRIVATE Catch2::Catch2
)

add_executable(test_slime_behavior
    tests/test_slime_behavior.cpp
)

target_link_libraries(test_slime_behavior
    PRIVATE enemy_slime
    PRIVATE Catch2::Catch2
)

# Register with CTest
add_test(NAME test_slime_spec COMMAND test_slime_spec)
add_test(NAME test_slime_behavior COMMAND test_slime_behavior) 