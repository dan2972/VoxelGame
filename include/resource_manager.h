#pragma once

#include <unordered_map>
#include <string>
#include <memory>

#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/mesh.h"

class ResourceManager
{
public:
    ResourceManager();
    ~ResourceManager();

    ResourceManager(const ResourceManager& other) = delete;
    ResourceManager(ResourceManager&& other) = delete;
    ResourceManager& operator=(const ResourceManager& other) = delete;
    ResourceManager& operator=(ResourceManager&& other) = delete;

    gfx::Shader* loadShader(const std::string& name, const std::string& vertPath, const std::string& fragPath, const std::string& geomPath = std::string());
    gfx::Shader* addShader(const std::string& name, gfx::Shader&& shader);
    gfx::Shader* getShader(const std::string& name) const;
    void removeShader(const std::string& name);
    
    gfx::Texture* loadTexture(const std::string& name, const std::string& path, GLint internalFormat = GL_SRGB_ALPHA);
    gfx::Texture* addTexture(const std::string& name, gfx::Texture&& texture);
    gfx::Texture* getTexture(const std::string& name) const;
    void removeTexture(const std::string& name);

    gfx::Mesh* addMesh(const std::string& name, const std::vector<float>& vertices, const std::vector<unsigned int>& indices, const std::vector<unsigned int>& dims);
    gfx::Mesh* addMesh(const std::string& name, gfx::Mesh&& mesh);
    gfx::Mesh* getMesh(const std::string& name) const;
    void removeMesh(const std::string& name);
private:
    std::unordered_map<std::string, std::unique_ptr<gfx::Shader>> m_shaders;
    std::unordered_map<std::string, std::unique_ptr<gfx::Texture>> m_textures;
    std::unordered_map<std::string, std::unique_ptr<gfx::Mesh>> m_meshes;
};