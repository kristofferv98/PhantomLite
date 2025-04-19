#!/bin/bash

# build_and_run.sh - Build and run the game
set -e  # Exit immediately if a command exits with a non-zero status

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${GREEN}Building game...${NC}"

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

# Step 1: Build the game
if cmake --build build --target game; then
    echo -e "${GREEN}Build successful!${NC}"
    
    # Step 2: Run the game
    echo -e "${GREEN}Running game...${NC}"
    ./build/src/game
else
    echo -e "${RED}Build failed!${NC}"
    exit 1
fi 