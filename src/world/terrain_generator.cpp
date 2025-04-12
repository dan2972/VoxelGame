#include "world/terrain_generator.h"

int TerrainGenerator::s_seed = 0;

TerrainGenerator::TerrainGenerator()
{
    m_noise.SetSeed(s_seed);
    m_noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    m_noise.SetFrequency(0.004f);
    m_noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    m_noise.SetFractalOctaves(5);
    m_noise.SetFractalLacunarity(2.0f);
    m_noise.SetFractalGain(0.5f);
}

float TerrainGenerator::getNoise(float x, float y) const
{
    float val = m_noise.GetNoise(x, y);
    val = val * val * val;
    val = val / (1 + std::abs(val));
    return val;
}

float TerrainGenerator::getNoise(const glm::vec3& pos) const
{
    float val = m_noise.GetNoise(pos.x, pos.y);
    val = val * val * val;
    val = val / (1 + std::abs(val));
    return val;
}