#pragma once

#include <unordered_map>
#include <string>

#include "graphics/shader.h"
#include "graphics/texture.h"

class ResourceManager
{
public:
    ResourceManager();
    ~ResourceManager();

    ResourceManager(const ResourceManager& other) = delete;
    ResourceManager(ResourceManager&& other) = delete;
    ResourceManager& operator=(const ResourceManager& other) = delete;
    ResourceManager& operator=(ResourceManager&& other) = delete;

    gfx::Shader* addShader(const std::string& name, const std::string& vertPath, const std::string& fragPath, const std::string& geomPath = std::string());
    gfx::Shader* addShader(const std::string& name, gfx::Shader&& shader);
    gfx::Shader* getShader(const std::string& name) const;
    void removeShader(const std::string& name);
    
    gfx::Texture* addTexture(const std::string& name, const std::string& path, GLint internalFormat = GL_SRGB_ALPHA);
    gfx::Texture* addTexture(const std::string& name, gfx::Texture&& texture);
    gfx::Texture* getTexture(const std::string& name) const;
    void removeTexture(const std::string& name);
private:
    std::unordered_map<std::string, gfx::Shader> m_shaders;
    std::unordered_map<std::string, gfx::Texture> m_textures;
};