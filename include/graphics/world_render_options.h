#pragma once

#include "world/chunk.h"

struct RenderOptions
{
    int renderDistance = 5;
    bool useAO = true;
    bool useSmoothLighting = true;
    bool showChunkBorder = false;
    bool showSunLightLevels = false;
    bool showBlockLightLevels = false;
    float showLightLevelRadius = Chunk::CHUNK_SIZE / 2;
    float aoFactor = 0.5f;
};