#include "world/block_data.h"

std::unordered_map<int, BlockTextureData> BlockData::s_blockTextureMap;
std::unordered_map<int, BlockProperties> BlockData::s_blockDataMap;
std::unordered_map<std::string, BlockTexture> BlockData::stringToBlockTexture = 
{
    {"grass_top", BlockTexture::GrassTop},
    {"grass_side", BlockTexture::GrassSide},
    {"dirt", BlockTexture::Dirt},
    {"stone", BlockTexture::Stone},
    {"wood_planks", BlockTexture::WoodPlanks},
    {"sand", BlockTexture::Sand},
    {"water", BlockTexture::Water}
};