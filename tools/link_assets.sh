#!/bin/bash
# Simple script to symlink assets to the build directory

# Check if BUILD_DIR is provided as an argument, otherwise use default
BUILD_DIR=${1:-"build"}

# Remove existing symlink if it exists
if [ -L "$BUILD_DIR/assets" ]; then
    rm "$BUILD_DIR/assets"
fi

# Create symlink
ln -sf "$(pwd)/assets" "$BUILD_DIR/assets"

echo "Assets symlink created successfully in $BUILD_DIR/assets" 