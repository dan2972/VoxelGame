#pragma once
#include "perlin_noise.h"
class PerlinGenerator {
public:
    static void initialize(unsigned int seed) {
        noise_seed = seed;
        perlin = siv::PerlinNoise{ noise_seed };
    }

    static double getValueAt(float x, float y, float multiplier, int octaves) {
        return perlin.octave2D_01(x * multiplier, y * multiplier, octaves);
    }

private:
    static siv::PerlinNoise::seed_type noise_seed;
    static siv::PerlinNoise perlin;
};

