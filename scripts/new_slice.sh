#!/usr/bin/env bash
set -e
slice=$1
[[ -z "$slice" ]] && { echo "Usage: $0 <slice>"; exit 1; }

base="src/features/$slice"
mkdir -p "$base"/{atoms,molecules,tests}
cat >"$base/$slice.hpp" <<EOF
/// $slice.hpp — public API for $slice slice
#pragma once
namespace $slice {
void init();
void update(float dt);
}  // namespace $slice
EOF
cat >"$base/$slice.cpp" <<EOF
/// $slice.cpp — organism for $slice slice
#include "$slice.hpp"
namespace $slice {
void init() {}
void update(float) {}
}  // namespace $slice
EOF
echo "Slice '$slice' scaffolded." 