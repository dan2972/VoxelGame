#include "resource_manager.h"
#include <spdlog/spdlog.h>

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

gfx::Shader* ResourceManager::addShader(const std::string& name, const std::string& vertPath, const std::string& fragPath, const std::string& geomPath)
{
    auto ret = m_shaders.try_emplace(name, vertPath, fragPath, geomPath);
    if (ret.second)
    {
        return &ret.first->second;
    }
    else
    {
        spdlog::warn("Shader with name {} already exists. Skipping addition.", name);
        return nullptr;
    }
}

gfx::Shader* ResourceManager::addShader(const std::string& name, gfx::Shader&& shader)
{
    auto ret = m_shaders.try_emplace(name, std::move(shader));
    if (ret.second)
    {
        return &ret.first->second;
    }
    else
    {
        spdlog::warn("Shader with name {} already exists. Skipping addition.", name);
        return nullptr;
    }
}

gfx::Shader* ResourceManager::getShader(const std::string& name) const
{
    auto it = m_shaders.find(name);
    if (it != m_shaders.end())
    {
        return const_cast<gfx::Shader*>(&it->second);
    }
    return nullptr;
}

void ResourceManager::removeShader(const std::string& name)
{
    m_shaders.erase(name);
}

gfx::Texture* ResourceManager::addTexture(const std::string& name, const std::string& path, GLint internalFormat)
{
    auto ret = m_textures.try_emplace(name, path, internalFormat);
    if (ret.second)
    {
        return &ret.first->second;
    }
    else
    {
        spdlog::warn("Texture with name {} already exists. Skipping addition.", name);
        return nullptr;
    }
}

gfx::Texture* ResourceManager::addTexture(const std::string& name, gfx::Texture&& texture)
{
    auto ret = m_textures.try_emplace(name, std::move(texture));
    if (ret.second)
    {
        return &ret.first->second;
    }
    else
    {
        spdlog::warn("Texture with name {} already exists. Skipping addition.", name);
        return nullptr;
    }
}

gfx::Texture* ResourceManager::getTexture(const std::string& name) const
{
    auto it = m_textures.find(name);
    if (it != m_textures.end())
    {
        return const_cast<gfx::Texture*>(&it->second);
    }
    return nullptr;
}

void ResourceManager::removeTexture(const std::string& name)
{
    m_textures.erase(name);
}
