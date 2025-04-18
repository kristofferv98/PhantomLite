# Add core library (engine bootstrap, shared, etc.)
file(GLOB SRC_CORE core/*.cpp)
add_library(core STATIC ${SRC_CORE})
target_link_libraries(core PUBLIC fmt::fmt raylib)
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Add feature slices
add_subdirectory(features/player)
add_subdirectory(features/world)
add_subdirectory(features/ui)
add_subdirectory(features/enemies)
add_subdirectory(features/enemy_slime)

# Add shared utilities
add_subdirectory(shared)

# Add core interface adapters
add_subdirectory(core)

# Game executable
add_executable(game core/main.cpp)
target_link_libraries(game 
    PRIVATE 
    core 
    core_interface
    shared_utils
    player_feature
    world_feature
    ui
    enemies_feature
    enemy_slime
)

# Unit tests (only if core/tests.cpp exists)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/core/tests.cpp")
    add_executable(unit_tests core/tests.cpp)
    target_link_libraries(unit_tests 
        PRIVATE 
        core 
        Catch2::Catch2WithMain
    )
endif() 