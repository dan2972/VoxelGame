#include "resource_loader.h"
#include "resource_manager.h"
#include "world/block_data.h"
#include "graphics/gfx/texture_atlas.h"

ResourceLoader::ResourceLoader(ResourceManager* resourceManager) : m_resourceManager(resourceManager) {}

void ResourceLoader::load(const glm::ivec2& frameBufferSize)
{
    if (!m_resourceManager)
        throw std::runtime_error("ResourceLoader: ResourceManager pointer is null.");

    // Load shaders
    m_resourceManager->loadShader("line", "res/shaders/line_renderer.vert", "res/shaders/line_renderer.frag");
    m_resourceManager->loadShader("font", "res/shaders/font_renderer.vert", "res/shaders/font_renderer.frag");
    m_resourceManager->loadShader("font_billboard", "res/shaders/font_renderer_billboard.vert", "res/shaders/font_renderer.frag");
    m_resourceManager->loadShader("screen_quad", "res/shaders/screen_quad.vert", "res/shaders/screen_quad.frag");
    m_resourceManager->loadShader("sky_color", "res/shaders/sky_color.vert", "res/shaders/sky_color.frag");
    m_resourceManager->loadShader("chunk", "res/shaders/terrain_chunk.vert", "res/shaders/terrain_chunk.frag");

    // Load Fonts
    auto fontRenderer = m_resourceManager->loadFontRenderer("default", "res/fonts/arial.ttf", 48);
    fontRenderer->preloadDefaultGlyphs();
    auto fontRendererBB = m_resourceManager->loadFontRenderer("default_billboard", "res/fonts/courier-mon.ttf", 48, true);
    fontRendererBB->preloadDefaultGlyphs();

    // Load RenderTargets and ScreenQuads
    m_resourceManager->addRenderTarget("game_target", frameBufferSize.x, frameBufferSize.y);
    m_resourceManager->addScreenQuad("game_quad");
    m_resourceManager->addScreenQuad("sky_quad");

    // Load LineRenderer
    m_resourceManager->addLineRenderer("default");

    // Setup Atlas
    auto atlas = m_resourceManager->addTextureAtlas<BlockTexture>("chunk_atlas", { .internalFilterMin = GL_NEAREST_MIPMAP_LINEAR, .internalFilterMag = GL_NEAREST });

    setupChunkAtlas();
    atlas->generateMipmaps(4);

    // setup BlockData
    BlockData::submitBlockData(BlockType::Air, { .isTransparent = true, .isLiquid = false, .isCube = false });
    BlockData::submitBlockData(BlockType::Grass);
    BlockData::submitBlockData(BlockType::Dirt);
    BlockData::submitBlockData(BlockType::Stone);
    BlockData::submitBlockData(BlockType::WoodPlanks);
    BlockData::submitBlockData(BlockType::Sand);
    BlockData::submitBlockData(BlockType::Water, { .isTranslucent = true, .isLiquid = true, .isCube = false });
    BlockData::submitBlockData(BlockType::Lamp, { .luminosity = 15 });
    BlockData::submitBlockTextureData(BlockType::Grass, BlockTextureData(BlockTexture::GrassTop, BlockTexture::Dirt, BlockTexture::GrassSide, BlockTexture::GrassSide, BlockTexture::GrassSide, BlockTexture::GrassSide));
    BlockData::submitBlockTextureData(BlockType::Dirt, BlockTextureData(BlockTexture::Dirt));
    BlockData::submitBlockTextureData(BlockType::Stone, BlockTextureData(BlockTexture::Stone));
    BlockData::submitBlockTextureData(BlockType::WoodPlanks, BlockTextureData(BlockTexture::WoodPlanks));
    BlockData::submitBlockTextureData(BlockType::Sand, BlockTextureData(BlockTexture::Sand));
    BlockData::submitBlockTextureData(BlockType::Water, BlockTextureData(BlockTexture::Water, BlockTexture::Water, BlockTexture::Water, BlockTexture::Water, BlockTexture::Water, BlockTexture::Water));
    BlockData::submitBlockTextureData(BlockType::Lamp, BlockTextureData(BlockTexture::Lamp));
}

void ResourceLoader::setupChunkAtlas()
{
    auto atlas = m_resourceManager->getTextureAtlas<BlockTexture>("chunk_atlas");
    if (!atlas)
    {
        spdlog::error("ResourceLoader: Chunk atlas not found.");
        return;
    }

    for (const auto& dirEntry : std::filesystem::directory_iterator("res/textures/"))
    {
        if (dirEntry.is_regular_file())
        {
            auto filePath = dirEntry.path().string();
            auto fileName = dirEntry.path().stem().string();
            auto it = BlockData::stringToBlockTexture.find(fileName);
            if (it == BlockData::stringToBlockTexture.end())
            {
                spdlog::warn("ResourceLoader: Texture {} not found in StringToBlockTexture map.", fileName);
                continue;
            }
            BlockTexture blockTexture = it->second;
            spdlog::info("Loading texture: {}", filePath);
            auto[uvMin, uvMax] = atlas->addImgFromPath(blockTexture, filePath);
        }
    }
}