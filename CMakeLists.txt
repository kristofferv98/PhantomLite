
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

# Get absolute paths for scripts to avoid issues with spaces in paths
set(CONVERT_SVGS_SCRIPT "${CMAKE_SOURCE_DIR}/tools/convert_svgs.sh")
set(LINK_ASSETS_SCRIPT "${CMAKE_SOURCE_DIR}/tools/link_assets.sh")
set(BUILD_DIR "${CMAKE_BINARY_DIR}")

# Create a temporary script to execute the asset conversion to avoid shell escaping issues
file(WRITE "${CMAKE_BINARY_DIR}/run_asset_scripts.sh" "#!/bin/bash\n")
file(APPEND "${CMAKE_BINARY_DIR}/run_asset_scripts.sh" "\"${CONVERT_SVGS_SCRIPT}\"\n")
file(APPEND "${CMAKE_BINARY_DIR}/run_asset_scripts.sh" "\"${LINK_ASSETS_SCRIPT}\" \"${BUILD_DIR}\"\n")
execute_process(COMMAND chmod +x "${CMAKE_BINARY_DIR}/run_asset_scripts.sh")

# Asset conversion - use the temporary script that has proper path handling
add_custom_target(assets
    COMMAND "${CMAKE_BINARY_DIR}/run_asset_scripts.sh"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Converting SVG assets to PNG and creating symlink..."
)

# Make sure assets are built before the game
add_subdirectory(src)
add_subdirectory(tests)

# Add a dependency between the game target and assets target
add_dependencies(game assets) 

