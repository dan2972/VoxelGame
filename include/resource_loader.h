#pragma once

#include <string>
#include <filesystem>
#include "resource_manager.h"

class ResourceLoader
{
public:
    ResourceLoader(ResourceManager* resourceManager);
    ~ResourceLoader() = default;

    void load(const glm::ivec2& frameBufferSize);
private:
    ResourceManager* m_resourceManager = nullptr;

    void setupChunkAtlas();
};