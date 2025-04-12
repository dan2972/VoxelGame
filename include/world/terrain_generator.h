#pragma once

#include <FastNoiseLite.h>
#include <glm/glm.hpp>

class TerrainGenerator {
public:
    TerrainGenerator();
    ~TerrainGenerator() = default;

    float getNoise(float x, float y) const;
    float getNoise(const glm::vec3& pos) const;

    static int getSeed() { return s_seed; }
    static void setSeed(int seed) { s_seed = seed; }

private:
    static int s_seed;
    FastNoiseLite m_noise;
};