#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <typeindex>
#include <any>

#include "graphics/gfx/shader.h"
#include "graphics/gfx/texture.h"
#include "graphics/gfx/mesh.h"
#include "graphics/gfx/font_renderer.h"
#include "graphics/gfx/line_renderer.h"
#include "graphics/gfx/texture_atlas.h"
#include "graphics/gfx/render_target.h"
#include "graphics/gfx/screen_quad.h"

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

    gfx::FontRenderer* loadFontRenderer(const std::string& name, const std::string& fontPath, unsigned int fontSize, const gfx::TextureAtlasParams& atlasParams, bool useBillboard = false);
    gfx::FontRenderer* loadFontRenderer(const std::string& name, const std::string& fontPath, unsigned int fontSize, bool useBillboard = false);
    gfx::FontRenderer* addFontRenderer(const std::string& name, gfx::FontRenderer&& fontRenderer);
    gfx::FontRenderer* getFontRenderer(const std::string& name) const;
    void removeFontRenderer(const std::string& name);

    gfx::LineRenderer* addLineRenderer(const std::string& name);
    gfx::LineRenderer* addLineRenderer(const std::string& name, gfx::LineRenderer&& lineRenderer);
    gfx::LineRenderer* getLineRenderer(const std::string& name) const;
    void removeLineRenderer(const std::string& name);

    gfx::RenderTarget* addRenderTarget(const std::string& name, int width, int height, unsigned int internalFormat = GL_DEPTH24_STENCIL8);
    gfx::RenderTarget* addRenderTarget(const std::string& name, gfx::RenderTarget&& renderTarget);
    gfx::RenderTarget* getRenderTarget(const std::string& name) const;
    void removeRenderTarget(const std::string& name);

    gfx::ScreenQuad* addScreenQuad(const std::string& name);
    gfx::ScreenQuad* addScreenQuad(const std::string& name, gfx::ScreenQuad&& screenQuad);
    gfx::ScreenQuad* getScreenQuad(const std::string& name) const;
    void removeScreenQuad(const std::string& name);

    template <typename T>
    gfx::TextureAtlas<T>* addTextureAtlas(const std::string& name, const gfx::TextureAtlasParams& params);
    template <typename T>
    gfx::TextureAtlas<T>* addTextureAtlas(const std::string& name, gfx::TextureAtlas<T>&& atlas);
    template <typename T>
    gfx::TextureAtlas<T>* getTextureAtlas(const std::string& name) const;
    template <typename T>
    void removeTextureAtlas(const std::string& name);
private:
    std::unordered_map<std::string, std::unique_ptr<gfx::Shader>> m_shaders;
    std::unordered_map<std::string, std::unique_ptr<gfx::Texture>> m_textures;
    std::unordered_map<std::string, std::unique_ptr<gfx::Mesh>> m_meshes;
    std::unordered_map<std::string, std::unique_ptr<gfx::FontRenderer>> m_fontRenderers;
    std::unordered_map<std::string, std::unique_ptr<gfx::LineRenderer>> m_lineRenderers;
    std::unordered_map<std::string, std::unique_ptr<gfx::RenderTarget>> m_renderTargets;
    std::unordered_map<std::string, std::unique_ptr<gfx::ScreenQuad>> m_screenQuads;
    
    std::unordered_map<std::type_index, std::any> m_textureAtlasTypeMap;

    template<typename T>
    std::unordered_map<std::string, std::shared_ptr<gfx::TextureAtlas<T>>>& getTypedMap();
    template<typename T>
    const std::unordered_map<std::string, std::shared_ptr<gfx::TextureAtlas<T>>>* getTypedMapPtr() const;
};

template <typename T>
gfx::TextureAtlas<T> *ResourceManager::addTextureAtlas(const std::string &name, const gfx::TextureAtlasParams &params)
{
    auto& typedMap = getTypedMap<T>();
    auto ret = typedMap.try_emplace(name, std::make_shared<gfx::TextureAtlas<T>>(params));
    if (ret.second)
    {
        return ret.first->second.get();
    }
    else
    {
        spdlog::warn("TextureAtlas with name \"{}\" already exists. Skipping addition.", name);
        return nullptr;
    }
}

template <typename T>
gfx::TextureAtlas<T> *ResourceManager::addTextureAtlas(const std::string &name, gfx::TextureAtlas<T> &&atlas)
{
    auto& typedMap = getTypedMap<T>();
    auto ret = typedMap.try_emplace(name, std::make_shared<gfx::TextureAtlas<T>>(std::move(atlas)));
    if (ret.second)
    {
        return ret.first->second.get();
    }
    else
    {
        spdlog::warn("TextureAtlas with name \"{}\" already exists. Skipping addition.", name);
        return nullptr;
    }
}

template <typename T>
gfx::TextureAtlas<T> *ResourceManager::getTextureAtlas(const std::string &name) const
{
    auto typedMap = getTypedMapPtr<T>();
    if (typedMap == nullptr)
    {
        spdlog::warn("TextureAtlas with name \"{}\" not found.", name);
        return nullptr;
    }
    auto it = typedMap->find(name);
    if (it != typedMap->end())
    {
        return it->second.get();
    }
    spdlog::warn("TextureAtlas with name \"{}\" not found.", name);
    return nullptr;
}

template <typename T>
void ResourceManager::removeTextureAtlas(const std::string &name)
{
    auto& typedMap = getTypedMap<T>();
    typedMap.erase(name);
}

template<typename T>
std::unordered_map<std::string, std::shared_ptr<gfx::TextureAtlas<T>>>& ResourceManager::getTypedMap()
{
    std::type_index index(typeid(T));
    if (m_textureAtlasTypeMap.find(index) == m_textureAtlasTypeMap.end()) {
        m_textureAtlasTypeMap[index] = std::unordered_map<std::string, std::shared_ptr<gfx::TextureAtlas<T>>>{};
    }
    return *std::any_cast<std::unordered_map<std::string, std::shared_ptr<gfx::TextureAtlas<T>>>>(&m_textureAtlasTypeMap[index]);
}

template<typename T>
const std::unordered_map<std::string, std::shared_ptr<gfx::TextureAtlas<T>>>* ResourceManager::getTypedMapPtr() const
{
    std::type_index index(typeid(T));
    if (m_textureAtlasTypeMap.find(index) == m_textureAtlasTypeMap.end()) {
        return nullptr;
    }
    return std::any_cast<std::unordered_map<std::string, std::shared_ptr<gfx::TextureAtlas<T>>> const>(&m_textureAtlasTypeMap.at(index));
}