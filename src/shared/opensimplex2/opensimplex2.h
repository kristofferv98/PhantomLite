/// opensimplex2.h — Lightweight OpenSimplex2 noise implementation for our steering behaviors
/// Based on the C implementation of OpenSimplex2 from https://github.com/MarcoCiaramella/OpenSimplex2

#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <math.h>

namespace opensimplex2 {

// Constants
constexpr double STRETCH_2D = -0.211324865405187;   // (1 / sqrt(2 + 1) - 1) / 2
constexpr double SQUISH_2D  = 0.366025403784439;    // (sqrt(2 + 1) - 1) / 2
constexpr double NORM_2D = 1.0 / 47.0;

// Permutation lookup table
class Noise {
private:
    int16_t perm[256];
    int16_t perm_grad_index_2d[256];
    
    // Gradient tables for 2D
    static constexpr double GRAD_2D[8][2] = {
        { 0.130526192220052, 0.99144486137381 },
        { 0.38268343236509, 0.923879532511287 },
        { 0.608761429008721, 0.793353340291235 },
        { 0.793353340291235, 0.608761429008721 },
        { 0.923879532511287, 0.38268343236509 },
        { 0.99144486137381, 0.130526192220051 },
        { 0.99144486137381, -0.130526192220051 },
        { 0.923879532511287, -0.38268343236509 }
    };

public:
    Noise(int64_t seed = 0) {
        // Initialize permutation arrays
        int16_t source[256];
        for (int i = 0; i < 256; i++) {
            source[i] = i;
        }
        
        // Shuffle based on seed
        seed = seed * 6364136223846793005LL + 1442695040888963407LL;
        seed = seed * 6364136223846793005LL + 1442695040888963407LL;
        seed = seed * 6364136223846793005LL + 1442695040888963407LL;
        
        for (int i = 255; i >= 0; i--) {
            seed = seed * 6364136223846793005LL + 1442695040888963407LL;
            int r = (int)((seed + 31) % (i + 1));
            if (r < 0) r += (i + 1);
            perm[i] = source[r];
            perm_grad_index_2d[i] = (short)((perm[i] % 8) * 2);
            source[r] = source[i];
        }
    }

    double noise2(double x, double y) {
        // Place input coordinates onto grid
        double stretch_offset = (x + y) * STRETCH_2D;
        double xs = x + stretch_offset;
        double ys = y + stretch_offset;
        
        // Floor to get grid coordinates of rhombus super-cell origin
        int xsb = floor(xs);
        int ysb = floor(ys);
        
        // Skew out to get actual coordinates of rhombus origin
        double squish_offset = (xsb + ysb) * SQUISH_2D;
        double xb = xsb + squish_offset;
        double yb = ysb + squish_offset;
        
        // Compute grid coordinates relative to rhombus origin
        double xins = xs - xsb;
        double yins = ys - ysb;
        
        // Sum those together to get a value that determines which region we're in
        double in_sum = xins + yins;
        
        // Positions relative to origin point
        double dx0 = x - xb;
        double dy0 = y - yb;
        
        // We'll be defining these outside the loops
        double dx_ext, dy_ext;
        int xsv_ext, ysv_ext;
        
        double value = 0;
        
        // Calculate contribution from the first gradient
        double dx1 = dx0 - 1 - SQUISH_2D;
        double dy1 = dy0 - 0 - SQUISH_2D;
        double attn1 = 2 - dx1 * dx1 - dy1 * dy1;
        if (attn1 > 0) {
            attn1 *= attn1;
            int gi1 = perm_grad_index_2d[(perm[(xsb + 1) & 0xFF] + ysb) & 0xFF] & 0x0E;
            value += attn1 * attn1 * (GRAD_2D[gi1 >> 1][0] * dx1 + GRAD_2D[gi1 >> 1][1] * dy1);
        }
        
        // Second corner contribution
        double dx2 = dx0 - 0 - SQUISH_2D;
        double dy2 = dy0 - 1 - SQUISH_2D;
        double attn2 = 2 - dx2 * dx2 - dy2 * dy2;
        if (attn2 > 0) {
            attn2 *= attn2;
            int gi2 = perm_grad_index_2d[(perm[xsb & 0xFF] + (ysb + 1)) & 0xFF] & 0x0E;
            value += attn2 * attn2 * (GRAD_2D[gi2 >> 1][0] * dx2 + GRAD_2D[gi2 >> 1][1] * dy2);
        }
        
        // Third corner contribution (depends on in_sum)
        if (in_sum <= 1) {
            double zins = 1 - in_sum;
            if (zins > xins || zins > yins) {
                if (xins > yins) {
                    xsv_ext = xsb + 1;
                    ysv_ext = ysb - 1;
                    dx_ext = dx0 - 1;
                    dy_ext = dy0 + 1;
                } else {
                    xsv_ext = xsb - 1;
                    ysv_ext = ysb + 1;
                    dx_ext = dx0 + 1;
                    dy_ext = dy0 - 1;
                }
            } else {
                xsv_ext = xsb + 1;
                ysv_ext = ysb + 1;
                dx_ext = dx0 - 1 - 2 * SQUISH_2D;
                dy_ext = dy0 - 1 - 2 * SQUISH_2D;
            }
        } else {
            double zins = 2 - in_sum;
            if (zins < xins || zins < yins) {
                if (xins > yins) {
                    xsv_ext = xsb + 2;
                    ysv_ext = ysb + 0;
                    dx_ext = dx0 - 2 - 2 * SQUISH_2D;
                    dy_ext = dy0 + 0 - 2 * SQUISH_2D;
                } else {
                    xsv_ext = xsb + 0;
                    ysv_ext = ysb + 2;
                    dx_ext = dx0 + 0 - 2 * SQUISH_2D;
                    dy_ext = dy0 - 2 - 2 * SQUISH_2D;
                }
            } else {
                xsv_ext = xsb;
                ysv_ext = ysb;
                dx_ext = dx0;
                dy_ext = dy0;
            }
            xsb += 1;
            ysb += 1;
            dx0 = dx0 - 1 - 2 * SQUISH_2D;
            dy0 = dy0 - 1 - 2 * SQUISH_2D;
        }
        
        // Contribution from third corner
        double attn0 = 2 - dx0 * dx0 - dy0 * dy0;
        if (attn0 > 0) {
            attn0 *= attn0;
            int gi0 = perm_grad_index_2d[(perm[(xsb) & 0xFF] + ysb) & 0xFF] & 0x0E;
            value += attn0 * attn0 * (GRAD_2D[gi0 >> 1][0] * dx0 + GRAD_2D[gi0 >> 1][1] * dy0);
        }
        
        // Contribution from the final corner
        double attn_ext = 2 - dx_ext * dx_ext - dy_ext * dy_ext;
        if (attn_ext > 0) {
            attn_ext *= attn_ext;
            int gi_ext = perm_grad_index_2d[(perm[(xsv_ext) & 0xFF] + ysv_ext) & 0xFF] & 0x0E;
            value += attn_ext * attn_ext * (GRAD_2D[gi_ext >> 1][0] * dx_ext + GRAD_2D[gi_ext >> 1][1] * dy_ext);
        }
        
        // Scale and return
        return value * NORM_2D;
    }
    
    // Overloaded function for more octaves (used in wander behavior)
    double noise2(double x, double y, int octaves, double persistence) {
        double total = 0;
        double frequency = 1;
        double amplitude = 1;
        double max_value = 0;  // Used for normalizing result to 0.0 - 1.0
        
        for(int i = 0; i < octaves; i++) {
            total += noise2(x * frequency, y * frequency) * amplitude;
            
            max_value += amplitude;
            
            amplitude *= persistence;
            frequency *= 2;
        }
        
        return total / max_value;
    }
};

} // namespace opensimplex2 