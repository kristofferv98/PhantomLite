/// math_utils.hpp — Common math utilities for use across slices
#pragma once
#include <raylib.h>
#include <cmath>

namespace shared {
namespace math {

/**
 * Calculate the distance between two points
 * PERF: ~0.01μs - Simple vector math
 */
inline float distance(const Vector2& a, const Vector2& b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return std::sqrt(dx * dx + dy * dy);
}

/**
 * Normalize a vector to unit length
 * PERF: ~0.02μs - Square root operation
 */
inline Vector2 normalize(const Vector2& v) {
    float len = std::sqrt(v.x * v.x + v.y * v.y);
    if (len > 0) {
        return { v.x / len, v.y / len };
    }
    return { 0.0f, 0.0f };
}

/**
 * Linearly interpolate between two values
 * PERF: ~0.005μs - Simple arithmetic
 */
inline float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

/**
 * Clamp a value between min and max
 * PERF: ~0.003μs - Simple comparison
 */
inline float clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

/**
 * Smoothly interpolate between values (ease in/out)
 * PERF: ~0.01μs - Multiple arithmetic ops
 */
inline float smooth_step(float a, float b, float t) {
    t = clamp((t - a) / (b - a), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

} // namespace math
} // namespace shared 