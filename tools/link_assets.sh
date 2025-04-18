#!/bin/bash
# Simple script to symlink assets to the build directory

# Check if BUILD_DIR is provided as an argument, otherwise use default
BUILD_DIR="${1:-build}"

# Remove existing symlink if it exists
if [ -L "${BUILD_DIR}/assets" ]; then
    rm "${BUILD_DIR}/assets"
fi

# Create symlink using absolute paths to avoid pwd issues with spaces
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "${SCRIPT_DIR}")"
ASSETS_DIR="${PROJECT_DIR}/assets"

# Create symlink with properly quoted paths
ln -sf "${ASSETS_DIR}" "${BUILD_DIR}/assets"

echo "Assets symlink created successfully in ${BUILD_DIR}/assets" 