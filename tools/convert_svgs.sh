#!/usr/bin/env bash
set -e

# Script directory and project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "${SCRIPT_DIR}")"

# Make sure the output directories exist
mkdir -p "${PROJECT_DIR}/assets/sprites"
mkdir -p "${PROJECT_DIR}/assets/tiles"
mkdir -p "${PROJECT_DIR}/assets/ui"

# SVG to PNG conversion function with enhanced transparency support
convert_svg() {
    local src_dir="$1"
    local dest_dir="$2"
    local width="$3"
    local height="$4"
    
    echo "Converting SVGs from ${src_dir} to PNGs in ${dest_dir}..."
    
    # Check for inkscape or use alternative for macOS conversion
    if command -v inkscape &> /dev/null; then
        echo "Using Inkscape for SVG conversion"
        find "${src_dir}" -name "*.svg" -type f | while read -r svg; do
            name=$(basename "${svg}" .svg)
            echo "Converting ${svg} to PNG..."
            inkscape "${svg}" --export-type="png" --export-filename="${dest_dir}/${name}.png" \
                --export-width=${width} --export-height=${height} --export-background-opacity=0 \
                --export-png-color-mode=RGBA_8
        done
    elif command -v rsvg-convert &> /dev/null; then
        echo "Using rsvg-convert for SVG conversion"
        find "${src_dir}" -name "*.svg" -type f | while read -r svg; do
            name=$(basename "${svg}" .svg)
            echo "Converting ${svg} to PNG..."
            # -a flag enables alpha channel transparency
            rsvg-convert -w ${width} -h ${height} -a -f png "${svg}" -o "${dest_dir}/${name}.png"
        done
    elif command -v convert &> /dev/null; then
        echo "Using ImageMagick for SVG conversion"
        find "${src_dir}" -name "*.svg" -type f | while read -r svg; do
            name=$(basename "${svg}" .svg)
            echo "Converting ${svg} to PNG..."
            # -background none ensures transparency is preserved
            convert -background none -alpha set -size ${width}x${height} "${svg}" PNG32:"${dest_dir}/${name}.png"
        done
    else
        echo "Error: No SVG conversion tool found. Please install Inkscape, librsvg, or ImageMagick."
        exit 1
    fi
}

# Convert sprites (64x64)
if [ -d "${PROJECT_DIR}/assets_src/sprites" ]; then
    convert_svg "${PROJECT_DIR}/assets_src/sprites" "${PROJECT_DIR}/assets/sprites" 64 64
fi

# Convert UI elements (32x32)
if [ -d "${PROJECT_DIR}/assets_src/ui" ]; then
    convert_svg "${PROJECT_DIR}/assets_src/ui" "${PROJECT_DIR}/assets/ui" 32 32
fi

# Convert standard tiles (32x32)
if [ -d "${PROJECT_DIR}/assets_src/tiles" ]; then
    # Remove any existing PNGs first to avoid issues with old files
    rm -f "${PROJECT_DIR}/assets/tiles/"*.png
    
    # Process all regular tiles except tree
    find "${PROJECT_DIR}/assets_src/tiles" -name "*.svg" -type f | while read -r svg; do
        name=$(basename "${svg}" .svg)
        if [ "${name}" != "tree" ]; then
            if command -v inkscape &> /dev/null; then
                inkscape "${svg}" --export-type="png" --export-filename="${PROJECT_DIR}/assets/tiles/${name}.png" \
                    --export-width=32 --export-height=32 --export-background-opacity=0 \
                    --export-png-color-mode=RGBA_8
            elif command -v rsvg-convert &> /dev/null; then
                rsvg-convert -w 32 -h 32 -a -f png "${svg}" -o "${PROJECT_DIR}/assets/tiles/${name}.png"
            elif command -v convert &> /dev/null; then
                convert -background none -alpha set "${svg}" PNG32:"${PROJECT_DIR}/assets/tiles/${name}.png"
            fi
        fi
    done
    
    # Handle tree separately (64x64)
    if [ -f "${PROJECT_DIR}/assets_src/tiles/tree.svg" ]; then
        if command -v inkscape &> /dev/null; then
            inkscape "${PROJECT_DIR}/assets_src/tiles/tree.svg" --export-type="png" --export-filename="${PROJECT_DIR}/assets/tiles/tree.png" \
                --export-width=64 --export-height=64 --export-background-opacity=0 \
                --export-png-color-mode=RGBA_8
        elif command -v rsvg-convert &> /dev/null; then
            rsvg-convert -w 64 -h 64 -a -f png "${PROJECT_DIR}/assets_src/tiles/tree.svg" -o "${PROJECT_DIR}/assets/tiles/tree.png"
        elif command -v convert &> /dev/null; then
            convert -background none -alpha set "${PROJECT_DIR}/assets_src/tiles/tree.svg" PNG32:"${PROJECT_DIR}/assets/tiles/tree.png"
        fi
    fi
fi

echo "SVG conversion complete!" 