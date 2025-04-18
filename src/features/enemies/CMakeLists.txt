cmake_minimum_required(VERSION 3.16)

# Define the enemies feature library
add_library(enemies_feature
    types.cpp
    types.hpp
    behavior_atoms.cpp
    behavior_atoms.hpp
)

# Include directories
target_include_directories(enemies_feature 
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/src
)

# Link dependencies
target_link_libraries(enemies_feature
    PUBLIC
        raylib
        world_feature  # Required for world::is_walkable and world::world_to_screen
)

# Set C++ standard
set_target_properties(enemies_feature PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
) 