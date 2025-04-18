#!/bin/bash
set -e

# Get the absolute path of the script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "${SCRIPT_DIR}"

echo "=== Cleaning build directory ==="
rm -rf build
mkdir -p build

echo "=== Installing dependencies with Conan ==="
conan install . --output-folder=build --build=missing

echo "=== Configuring with CMake ==="
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cd ..

echo "=== Building the game ==="
cmake --build build --target game

echo "=== Running the game ==="
"${SCRIPT_DIR}/build/src/game" 