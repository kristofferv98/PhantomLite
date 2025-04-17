file(GLOB_RECURSE SRC_CORE core/*.cpp)
add_library(core STATIC ${SRC_CORE})
target_link_libraries(core PUBLIC fmt::fmt raylib)

# --- features ---
add_subdirectory(features/player)

# --- executable ---
add_executable(game core/main.cpp)
target_link_libraries(game PRIVATE core player_feature) 