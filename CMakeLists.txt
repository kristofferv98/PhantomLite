cmake_minimum_required(VERSION 3.26)
project(phantomlite LANGUAGES CXX)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find dependencies - search in the build directory for CMake config files
set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
find_package(fmt REQUIRED)
find_package(raylib REQUIRED)
find_package(Catch2 REQUIRED)

# Include dirs
include_directories(${raylib_INCLUDE_DIRS} ${fmt_INCLUDE_DIRS} ${Catch2_INCLUDE_DIRS})

# Asset conversion - run svg to png conversion script
add_custom_target(assets
    COMMAND ${CMAKE_SOURCE_DIR}/tools/convert_svgs.sh
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Converting SVG assets to PNG..."
)

# Make sure assets are built before the game
add_subdirectory(src)
add_subdirectory(tests)

# Add a dependency between the game target and assets target
add_dependencies(game assets) 