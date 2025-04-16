#pragma once

#include "world/chunk.h"

struct RenderOptions
{
    bool useAO = true;
    bool useSmoothLighting = true;
    bool showChunkBorder = false;
    bool showSunLightLevels = false;
    bool showBlockLightLevels = false;
    float showLightLevelRadius = Chunk::CHUNK_SIZE / 2;
    float aoFactor = 0.65f;
};