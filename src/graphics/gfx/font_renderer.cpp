#include "graphics/gfx/font_renderer.h"
#include <glad/glad.h>

namespace gfx
{
    FT_Library FontRenderer::s_ftlib = nullptr;
    unsigned int FontRenderer::s_instanceCount = 0;

    FontRenderer::FontRenderer()
    {
        initFTLib();
        ++s_instanceCount;
    }

    FontRenderer::FontRenderer(bool useBillboard) : m_useBillboard(useBillboard)
    {
        initFTLib();
        ++s_instanceCount;
    }

    FontRenderer::~FontRenderer()
    {
        if (m_face)
        {
            FT_Done_Face(m_face);
        }
        --s_instanceCount;
        if (s_instanceCount == 0 && s_ftlib)
        {
            FT_Done_FreeType(s_ftlib);
            spdlog::info("Deinitialized FreeType Library. This should have only been called once.");
        }
    }
    
    FontRenderer::FontRenderer(FontRenderer&& other) noexcept
        : m_fontSize(other.m_fontSize),
          m_vertices(std::move(other.m_vertices)),
          m_verticesBB(std::move(other.m_verticesBB)),
          m_indices(std::move(other.m_indices)),
          m_curVertex(other.m_curVertex),
          m_curIndex(other.m_curIndex),
          m_textureAtlas(std::move(other.m_textureAtlas)),
          m_characters(std::move(other.m_characters)),
          m_batchMesh(std::move(other.m_batchMesh)),
          m_meshInitialized(other.m_meshInitialized),
          m_face(other.m_face)
    {
        other.m_face = nullptr;
    }

    FontRenderer& FontRenderer::operator=(FontRenderer&& other) noexcept
    {
        if (this != &other)
        {
            m_fontSize = other.m_fontSize;
            m_vertices = std::move(other.m_vertices);
            m_verticesBB = std::move(other.m_verticesBB);
            m_indices = std::move(other.m_indices);
            m_curVertex = other.m_curVertex;
            m_curIndex = other.m_curIndex;
            m_textureAtlas = std::move(other.m_textureAtlas);
            m_characters = std::move(other.m_characters);
            m_batchMesh = std::move(other.m_batchMesh);
            m_meshInitialized = other.m_meshInitialized;
            m_face = other.m_face;
            
            other.m_face = nullptr;
        }
        return *this;
    }

    void FontRenderer::loadFont(const std::string& fontPath, unsigned int fontSize)
    {
        load(fontPath, fontSize);
        m_textureAtlas = TextureAtlas<wchar_t>(
            {.internalFilterMin = GL_NEAREST, .internalFilterMag = GL_NEAREST, .internalFormat = GL_RED, .format = GL_RED}
        );
    }

    void FontRenderer::loadFont(const std::string& fontPath, unsigned int fontSize, const TextureAtlasParams& atlasParams)
    {
        load(fontPath, fontSize);
        m_textureAtlas = TextureAtlas<wchar_t>(atlasParams);
    }

    void FontRenderer::preloadGlyphs(const std::wstring &text)
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        for (wchar_t c : text)
        {
            if (FT_Load_Char(m_face, c, FT_LOAD_RENDER))
                continue;
            
            if (!m_textureAtlas.has(c)) {
                if (!setupGlyph(c, m_face->glyph))
                    continue;
                m_characters[c] = {
                    glm::ivec2(m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows),
                    glm::ivec2(m_face->glyph->bitmap_left, m_face->glyph->bitmap_top),
                    static_cast<unsigned int>(m_face->glyph->advance.x >> 6)
                };
            }
        }
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    }

    void FontRenderer::preloadGlyphs(const std::string &text)
    {
        std::wstring wText(text.begin(), text.end());
        preloadGlyphs(wText);
    }

    void FontRenderer::preloadDefaultGlyphs()
    {
        static std::wstring defaultGlyphs = L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()_+-=[]{}|;':\",.<>?/`~ ";
        preloadGlyphs(defaultGlyphs);
    }

    void FontRenderer::beginBatch()
    {
        if (!m_meshInitialized)
        {
            if (m_useBillboard)
                m_batchMesh.populate({}, {}, { 3, 3, 4, 2 }, GL_DYNAMIC_DRAW, GL_DYNAMIC_DRAW);
            else
                m_batchMesh.populate({}, {}, { 3, 4, 2 }, GL_DYNAMIC_DRAW, GL_DYNAMIC_DRAW);
            m_meshInitialized = true;
        }
        m_curVertex = 0;
        m_curIndex = 0;
    }

    void FontRenderer::addText(const std::wstring &text, float x, float y, float scale, const glm::vec4 &color, bool center)
    {
        addText(text, x, y, 0.0f, scale, color);
    }

    void FontRenderer::addText(const std::wstring &text, float x, float y, float z, float scale, const glm::vec4 &color, bool center)
    {
        if (m_useBillboard)
            return;
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        if (center)
        {
            float textWidth = 0.0f;
            for (wchar_t c : text)
            {
                if (m_characters.contains(c))
                {
                    textWidth += (m_characters[c].advance) * scale;
                }
            }
            x -= textWidth / 2.0f;
        }
        for (wchar_t c : text)
        {
            if (!m_characters.contains(c))
            {
                if (FT_Load_Char(m_face, c, FT_LOAD_RENDER))
                    continue;
                
                if (!m_textureAtlas.has(c)) {
                    if (!setupGlyph(c, m_face->glyph))
                        continue;
                    m_characters[c] = {
                        glm::ivec2(m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows),
                        glm::ivec2(m_face->glyph->bitmap_left, m_face->glyph->bitmap_top),
                        static_cast<unsigned int>(m_face->glyph->advance.x >> 6)
                    };
                }
            }

            Character& ch = m_characters[c];

            if (!m_textureAtlas.has(c)) { // for cases where the character is not renderable (e.g. space)
                x += ch.advance * scale;
                continue;
            }

            auto [uvMin, uvMax] = m_textureAtlas.get(c);

            float xPos = x + (ch.bearing.x * scale);
            float yPos = y - (ch.size.y - ch.bearing.y) * scale;

            float w = ch.size.x * scale;
            float h = ch.size.y * scale;

            if (m_curVertex + 4 >= m_vertices.size())
                m_vertices.resize(m_curVertex + 4);
            if (m_curIndex + 6 >= m_indices.size())
                m_indices.resize(m_curIndex + 6);
            
            m_vertices[m_curVertex + 0] = { glm::vec3(xPos, yPos + h, z), color, uvMin };
            m_vertices[m_curVertex + 1] = { glm::vec3(xPos, yPos, z), color, glm::vec2(uvMin.x, uvMax.y) };
            m_vertices[m_curVertex + 2] = { glm::vec3(xPos + w, yPos, z), color, uvMax };
            m_vertices[m_curVertex + 3] = { glm::vec3(xPos + w, yPos + h, z), color, glm::vec2(uvMax.x, uvMin.y) };

            m_indices[m_curIndex + 0] = m_curVertex + 0;
            m_indices[m_curIndex + 1] = m_curVertex + 1;
            m_indices[m_curIndex + 2] = m_curVertex + 2;
            m_indices[m_curIndex + 3] = m_curVertex + 0;
            m_indices[m_curIndex + 4] = m_curVertex + 2;
            m_indices[m_curIndex + 5] = m_curVertex + 3;

            m_curVertex += 4;
            m_curIndex += 6;

            x += ch.advance * scale;
        }
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    }

    void FontRenderer::addText(const std::wstring &text, const glm::vec3 &pos, float scale, const glm::vec4 &color, bool center)
    {
        addText(text, pos.x, pos.y, pos.z, scale, color, center);
    }

    void FontRenderer::addText(const std::string &text, float x, float y, float scale, const glm::vec4 &color, bool center)
    {
        std::wstring wText(text.begin(), text.end());
        addText(wText, x, y, scale, color, center);
    }

    void FontRenderer::addText(const std::string &text, float x, float y, float z, float scale, const glm::vec4 &color, bool center)
    {
        std::wstring wText(text.begin(), text.end());
        addText(wText, x, y, z, scale, color, center);
    }

    void FontRenderer::addText(const std::string &text, const glm::vec3 &pos, float scale, const glm::vec4 &color, bool center)
    {
        std::wstring wText(text.begin(), text.end());
        addText(wText, pos.x, pos.y, pos.z, scale, color, center);
    }

    void FontRenderer::addTextBB(const std::wstring& text, const glm::vec3& pos, const glm::vec3& center, float scale, const glm::vec4& color, bool textCentered)
    {
        if (!m_useBillboard)
            return;
        float x = pos.x;
        float y = pos.y;
        float z = pos.z;
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        if (textCentered)
        {
            float textWidth = 0.0f;
            for (wchar_t c : text)
            {
                if (m_characters.contains(c))
                {
                    textWidth += (m_characters[c].advance) * scale;
                }
            }
            x -= textWidth / 2.0f;
        }
        for (wchar_t c : text)
        {
            if (!m_characters.contains(c))
            {
                if (FT_Load_Char(m_face, c, FT_LOAD_RENDER))
                    continue;
                
                if (!m_textureAtlas.has(c)) {
                    if (!setupGlyph(c, m_face->glyph))
                        continue;
                    m_characters[c] = {
                        glm::ivec2(m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows),
                        glm::ivec2(m_face->glyph->bitmap_left, m_face->glyph->bitmap_top),
                        static_cast<unsigned int>(m_face->glyph->advance.x >> 6)
                    };
                }
            }

            Character& ch = m_characters[c];

            if (!m_textureAtlas.has(c)) { // for cases where the character is not renderable (e.g. space)
                x += ch.advance * scale;
                continue;
            }

            auto [uvMin, uvMax] = m_textureAtlas.get(c);

            float xPos = x + (ch.bearing.x * scale);
            float yPos = y - (ch.size.y - ch.bearing.y) * scale;

            float w = ch.size.x * scale;
            float h = ch.size.y * scale;

            if (m_curVertex + 4 >= m_verticesBB.size())
                m_verticesBB.resize(m_curVertex + 4);
            if (m_curIndex + 6 >= m_indices.size())
                m_indices.resize(m_curIndex + 6);
            
            m_verticesBB[m_curVertex + 0] = { glm::vec3(xPos, yPos + h, z), center, color, uvMin };
            m_verticesBB[m_curVertex + 1] = { glm::vec3(xPos, yPos, z), center, color, glm::vec2(uvMin.x, uvMax.y) };
            m_verticesBB[m_curVertex + 2] = { glm::vec3(xPos + w, yPos, z), center, color, uvMax };
            m_verticesBB[m_curVertex + 3] = { glm::vec3(xPos + w, yPos + h, z), center, color, glm::vec2(uvMax.x, uvMin.y) };

            m_indices[m_curIndex + 0] = m_curVertex + 0;
            m_indices[m_curIndex + 1] = m_curVertex + 1;
            m_indices[m_curIndex + 2] = m_curVertex + 2;
            m_indices[m_curIndex + 3] = m_curVertex + 0;
            m_indices[m_curIndex + 4] = m_curVertex + 2;
            m_indices[m_curIndex + 5] = m_curVertex + 3;

            m_curVertex += 4;
            m_curIndex += 6;

            x += ch.advance * scale;
        }
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    }

    void FontRenderer::addTextBB(const std::string& text, const glm::vec3& pos, const glm::vec3& center, float scale, const glm::vec4& color, bool textCentered)
    {
        std::wstring wText(text.begin(), text.end());
        addTextBB(wText, pos, center, scale, color, textCentered);
    }

    void FontRenderer::clearText()
    {
        m_vertices.clear();
        m_verticesBB.clear();
        m_indices.clear();
        m_curVertex = 0;
        m_curIndex = 0;
    }

    void FontRenderer::draw(bool bindVAO)
    {
        if (m_curVertex == 0 || m_curIndex == 0)
            return;

        m_textureAtlas.use();

        float* verticesFloats;
        size_t verticesSize;
        if (m_useBillboard) {
            verticesFloats = reinterpret_cast<float*>(m_verticesBB.data());
            verticesSize = m_curVertex * sizeof(VertexBB) / sizeof(float);
        } else {
            verticesFloats = reinterpret_cast<float*>(m_vertices.data());
            verticesSize = m_curVertex * sizeof(Vertex) / sizeof(float);
        }
        auto verticesVec = std::vector<float>(verticesFloats, verticesFloats + verticesSize);
        m_batchMesh.updateVertexBuffer(verticesVec, verticesSize, bindVAO);
        m_batchMesh.updateIndexBuffer(m_indices, m_curIndex, false);
        m_batchMesh.draw(m_curIndex);
    }

    void FontRenderer::initFTLib()
    {
        if (s_ftlib)
            return;
        spdlog::info("Initialized FreeType Library. This should have only been called once.");
        if (FT_Init_FreeType(&s_ftlib))
        {
            throw std::runtime_error("Failed to initialize FreeType library.");
        }
    }

    void FontRenderer::load(const std::string &fontPath, unsigned int fontSize)
    {
        if (m_face)
        {
            FT_Done_Face(m_face);
        }
        if (FT_New_Face(s_ftlib, fontPath.c_str(), 0, &m_face))
        {
            throw std::runtime_error("Failed to load font: " + fontPath);
        }
        FT_Set_Pixel_Sizes(m_face, 0, fontSize);
        m_fontSize = fontSize;
    }

    bool FontRenderer::setupGlyph(wchar_t character, FT_GlyphSlot glyph)
    {
        unsigned int width = glyph->bitmap.width;
        unsigned int height = glyph->bitmap.rows;
        
        if (glyph->bitmap.buffer != nullptr) {
            auto uvs = m_textureAtlas.add(character, glyph->bitmap.buffer, width, height);
            return uvs.first != glm::vec2(0) && uvs.second != glm::vec2(0);
        }
        return true;  // for cases where the character is not renderable (e.g. space)
    }
}