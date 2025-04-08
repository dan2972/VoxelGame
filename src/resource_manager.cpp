#include "resource_manager.h"
#include <spdlog/spdlog.h>

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

gfx::Shader* ResourceManager::loadShader(const std::string& name, const std::string& vertPath, const std::string& fragPath, const std::string& geomPath)
{
    auto ret = m_shaders.try_emplace(name, std::make_unique<gfx::Shader>(vertPath, fragPath, geomPath));
    if (ret.second)
    {
        return ret.first->second.get();
    }
    else
    {
        spdlog::warn("Shader with name \"{}\" already exists. Skipping addition.", name);
        return nullptr;
    }
}

gfx::Shader* ResourceManager::addShader(const std::string& name, gfx::Shader&& shader)
{
    auto ret = m_shaders.try_emplace(name, std::make_unique<gfx::Shader>(std::move(shader)));
    if (ret.second)
    {
        return ret.first->second.get();
    }
    else
    {
        spdlog::warn("Shader with name \"{}\" already exists. Skipping addition.", name);
        return nullptr;
    }
}

gfx::Shader* ResourceManager::getShader(const std::string& name) const
{
    auto it = m_shaders.find(name);
    if (it != m_shaders.end())
    {
        return it->second.get();
    }
    return nullptr;
}

void ResourceManager::removeShader(const std::string& name)
{
    m_shaders.erase(name);
}

gfx::Texture* ResourceManager::loadTexture(const std::string& name, const std::string& path, GLint internalFormat)
{
    auto ret = m_textures.try_emplace(name, std::make_unique<gfx::Texture>(path, internalFormat));
    if (ret.second)
    {
        return ret.first->second.get();
    }
    else
    {
        spdlog::warn("Texture with name \"{}\" already exists. Skipping addition.", name);
        return nullptr;
    }
}

gfx::Texture* ResourceManager::addTexture(const std::string& name, gfx::Texture&& texture)
{
    auto ret = m_textures.try_emplace(name, std::make_unique<gfx::Texture>(std::move(texture)));
    if (ret.second)
    {
        return ret.first->second.get();
    }
    else
    {
        spdlog::warn("Texture with name \"{}\" already exists. Skipping addition.", name);
        return nullptr;
    }
}

gfx::Texture* ResourceManager::getTexture(const std::string& name) const
{
    auto it = m_textures.find(name);
    if (it != m_textures.end())
    {
        return it->second.get();
    }
    return nullptr;
}

void ResourceManager::removeTexture(const std::string& name)
{
    m_textures.erase(name);
}

gfx::Mesh *ResourceManager::addMesh(const std::string &name, const std::vector<float> &vertices, const std::vector<unsigned int> &indices, const std::vector<unsigned int> &dims)
{
    auto ret = m_meshes.try_emplace(name, std::make_unique<gfx::Mesh>());
    if (ret.second)
    {
        ret.first->second->populate(vertices, indices, dims);
        return ret.first->second.get();
    }
    else
    {
        spdlog::warn("Mesh with name \"{}\" already exists. Skipping addition.", name);
        return nullptr;
    }
}

gfx::Mesh *ResourceManager::addMesh(const std::string &name, gfx::Mesh &&mesh)
{
    auto ret = m_meshes.try_emplace(name, std::make_unique<gfx::Mesh>(std::move(mesh)));
    if (ret.second)
    {
        return ret.first->second.get();
    }
    else
    {
        spdlog::warn("Mesh with name \"{}\" already exists. Skipping addition.", name);
        return nullptr;
    }
}

gfx::Mesh *ResourceManager::getMesh(const std::string &name) const
{
    auto it = m_meshes.find(name);
    if (it != m_meshes.end())
    {
        return it->second.get();
    }
    return nullptr;
}

void ResourceManager::removeMesh(const std::string &name)
{
    m_meshes.erase(name);
}

gfx::FontRenderer *ResourceManager::loadFontRenderer(const std::string &name, const std::string &fontPath, unsigned int fontSize, const gfx::TextureAtlasParams &atlasParams)
{
    auto ret = m_fontRenderers.try_emplace(name, std::make_unique<gfx::FontRenderer>());
    if (ret.second)
    {
        ret.first->second->loadFont(fontPath, fontSize, atlasParams);
        return ret.first->second.get();
    }
    else
    {
        spdlog::warn("FontRenderer with name \"{}\" already exists. Skipping addition.", name);
        return nullptr;
    }
}

gfx::FontRenderer *ResourceManager::loadFontRenderer(const std::string &name, const std::string &fontPath, unsigned int fontSize)
{
    auto ret = m_fontRenderers.try_emplace(name, std::make_unique<gfx::FontRenderer>());
    if (ret.second)
    {
        ret.first->second->loadFont(fontPath, fontSize);
        return ret.first->second.get();
    }
    else
    {
        spdlog::warn("FontRenderer with name \"{}\" already exists. Skipping addition.", name);
        return nullptr;
    }
}

gfx::FontRenderer *ResourceManager::addFontRenderer(const std::string &name, gfx::FontRenderer &&fontRenderer)
{
    auto ret = m_fontRenderers.try_emplace(name, std::make_unique<gfx::FontRenderer>(std::move(fontRenderer)));
    if (ret.second)
    {
        return ret.first->second.get();
    }
    else
    {
        spdlog::warn("FontRenderer with name \"{}\" already exists. Skipping addition.", name);
        return nullptr;
    }
}

gfx::FontRenderer *ResourceManager::getFontRenderer(const std::string &name) const
{
    auto it = m_fontRenderers.find(name);
    if (it != m_fontRenderers.end())
    {
        return it->second.get();
    }
    return nullptr;
}

void ResourceManager::removeFontRenderer(const std::string &name)
{
    m_fontRenderers.erase(name);
}