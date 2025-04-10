#pragma once

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "texture_atlas.h"
#include "mesh.h"

namespace gfx
{
    class FontRenderer
    {
    public:
        FontRenderer();
        ~FontRenderer();

        FontRenderer(const FontRenderer&) = delete;
        FontRenderer& operator=(const FontRenderer&) = delete;
        FontRenderer(FontRenderer&&) noexcept;
        FontRenderer& operator=(FontRenderer&&) noexcept;

        void loadFont(const std::string& fontPath, unsigned int fontSize);
        void loadFont(const std::string& fontPath, unsigned int fontSize, const TextureAtlasParams& atlasParams);

        void preloadGlyphs(const std::wstring& text);
        void preloadGlyphs(const std::string& text);
        void preloadDefaultGlyphs();

        void beginBatch();
        
        void addText(const std::wstring& text, float x, float y, float scale, const glm::vec4& color);
        void addText(const std::wstring& text, float x, float y, float z, float scale, const glm::vec4& color);
        void addText(const std::string& text, float x, float y, float scale, const glm::vec4& color);
        void addText(const std::string& text, float x, float y, float z, float scale, const glm::vec4& color);

        void clearText();

        void draw(bool bindVAO = true);

        TextureAtlas<wchar_t>& getTextureAtlas() { return m_textureAtlas; }

    private:
        void load(const std::string& fontPath, unsigned int fontSize);
        bool setupGlyph(wchar_t character, FT_GlyphSlot glyph);

        struct Vertex
        {
            glm::vec3 position;
            glm::vec4 color;
            glm::vec2 texCoords;
        };

        struct Character
        {
            glm::ivec2 size;
            glm::ivec2 bearing;
            unsigned int advance;
        };

        unsigned int m_fontSize;
        std::vector<Vertex> m_vertices;
        std::vector<unsigned int> m_indices;
        unsigned int m_curVertex = 0;
        unsigned int m_curIndex = 0;

        TextureAtlas<wchar_t> m_textureAtlas;
        std::unordered_map<wchar_t, Character> m_characters;
        Mesh m_batchMesh;
        bool m_meshInitialized = false;

        static FT_Library m_ftlib;
        FT_Face m_face = nullptr;
    };
}