#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>
#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <stb_image.h>
#include <iostream>

namespace gfx
{
    struct TextureAtlasParams
    {
        unsigned int width = 128;
        unsigned int height = 128;
        GLuint internalFilter = GL_LINEAR;
        GLuint wrapFilter = GL_CLAMP_TO_EDGE;
        GLuint internalFormat = GL_RGBA;
        GLuint format = GL_RGBA;
    };

    template<typename T>
    class TextureAtlas
    {
    public:
        TextureAtlas(
            unsigned int width = 128,
            unsigned int height = 128,
            GLuint internalFilter = GL_LINEAR,
            GLuint wrapFilter = GL_CLAMP_TO_EDGE,
            GLuint internalFormat = GL_RGBA,
            GLuint format = GL_RGBA);

        TextureAtlas(const TextureAtlasParams& params);

        ~TextureAtlas();

        TextureAtlas(const TextureAtlas& other) = delete;
        TextureAtlas(TextureAtlas&& other) noexcept;
        TextureAtlas& operator=(const TextureAtlas& other) = delete;
        TextureAtlas& operator=(TextureAtlas&& other) noexcept;

        std::pair<glm::vec2, glm::vec2> addImgFromPath(const T& key, const std::string& path);

        // returns the uv coordinates of the texture in the atlas (topleft, bottomright)
        // returns {0,0} if the texture was not added successfully
        std::pair<glm::vec2, glm::vec2> add(const T& key, unsigned char* data, unsigned int width, unsigned int height);
        std::pair<glm::vec2, glm::vec2> get(const T& key) const;

        bool expandAtlas();

        bool has(const T& key) const;

        void use() const;
        void destroy();
    private:
        static int s_maxAtlasWidth;
        static int s_maxAtlasHeight;

        std::unordered_map<T, std::pair<glm::vec2, glm::vec2>> m_textureCoords;
        unsigned int m_id = 0;
        int m_width;
        int m_height;
        bool m_initialized = false;
        GLuint m_internalFilter;
        GLuint m_wrapFilter;
        GLuint m_internalFormat;
        GLenum m_format;

        std::vector<glm::ivec2> m_skyline;

        void init();

        void checkMaxTextureSize();

        bool findPosition(int rectWidth, int rectHeight, int &bestIndex, int& nextIndex, int &bestX, int &bestY) const;
        
        void updateSkyline(int bestIndex, int nextIndex, int bestX, int bestY, int rectWidth, int rectHeight);
    };

    template<typename T>
    int TextureAtlas<T>::s_maxAtlasWidth = 0;
    template<typename T>
    int TextureAtlas<T>::s_maxAtlasHeight = 0;


    template<typename T>
    TextureAtlas<T>::TextureAtlas(
        unsigned int width,
        unsigned int height,
        GLuint internalFilter,
        GLuint wrapFilter,
        GLuint internalFormat,
        GLuint format)
        : m_width(width), m_height(height), m_internalFilter(internalFilter), m_wrapFilter(wrapFilter), m_internalFormat(internalFormat), m_format(format)
    {
        checkMaxTextureSize();
    }

    template<typename T>
    TextureAtlas<T>::TextureAtlas(const TextureAtlasParams& params)
        : m_width(params.width), m_height(params.height), m_internalFilter(params.internalFilter), m_wrapFilter(params.wrapFilter), m_internalFormat(params.internalFormat), m_format(params.format)
    {
        checkMaxTextureSize();
    }

    template<typename T>
    TextureAtlas<T>::~TextureAtlas() {
        destroy();
    }

    template<typename T>
    TextureAtlas<T>::TextureAtlas(TextureAtlas&& other) noexcept
    {
        destroy();
        m_textureCoords = std::move(other.m_textureCoords);
        m_id = other.m_id;
        m_width = other.m_width;
        m_height = other.m_height;
        m_skyline = std::move(other.m_skyline);
        m_internalFilter = other.m_internalFilter;
        m_wrapFilter = other.m_wrapFilter;
        m_internalFormat = other.m_internalFormat;
        m_format = other.m_format;
        other.m_id = 0;
        other.m_width = 0;
        other.m_height = 0;
    }

    template<typename T>
    TextureAtlas<T>& TextureAtlas<T>::operator=(TextureAtlas&& other) noexcept
    {
        if (this != &other) {
            destroy();
            m_textureCoords = std::move(other.m_textureCoords);
            m_id = other.m_id;
            m_width = other.m_width;
            m_height = other.m_height;
            m_skyline = std::move(other.m_skyline);
            m_internalFilter = other.m_internalFilter;
            m_wrapFilter = other.m_wrapFilter;
            m_internalFormat = other.m_internalFormat;
            m_format = other.m_format;
            other.m_id = 0;
            other.m_width = 0;
            other.m_height = 0;
        }
        return *this;
    }

    template<typename T>
    std::pair<glm::vec2, glm::vec2> TextureAtlas<T>::addImgFromPath(const T& key, const std::string& path) {
        if (!m_initialized) {
            init();
        }

        if (m_textureCoords.contains(key)) {
            spdlog::warn("TextureAtlas: Texture already exists in atlas.");
            return { glm::vec2(0), glm::vec2(0) };
        }

        int width, height, channels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);
        if (!data) {
            spdlog::error("TextureAtlas: Failed to load texture from file: {}", path);
            return { glm::vec2(0), glm::vec2(0) };
        }

        auto texCoords = add(key, data, width, height);
        stbi_image_free(data);
        return texCoords;
    }


    // returns the uv coordinates of the texture in the atlas (topleft, bottomright)
    // returns {0,0} if the texture was not added successfully
    template<typename T>
    std::pair<glm::vec2, glm::vec2> TextureAtlas<T>::add(const T& key, unsigned char* data, unsigned int width, unsigned int height) {
        if (!m_initialized) {
            init();
        }

        if (data == nullptr) {
            spdlog::warn("TextureAtlas: Data is null. Cannot add texture.");
            return { glm::vec2(0), glm::vec2(0) };
        }

        if (m_textureCoords.contains(key)) {
            spdlog::warn("TextureAtlas: Texture already exists in atlas.");
            return { glm::vec2(0), glm::vec2(0) };
        }

        if (width > s_maxAtlasWidth || height > s_maxAtlasHeight) {
            spdlog::warn("TextureAtlas: Texture size exceeds maximum atlas size.");
            return { glm::vec2(0), glm::vec2(0) };
        }

        int bestIndex, nextIndex, bestX, bestY;
        while (!findPosition(width, height, bestIndex, nextIndex, bestX, bestY)) {
            if (!expandAtlas()) {
                spdlog::error("TextureAtlas: Failed to resize atlas. Cannot add texture.");
                return { glm::vec2(0), glm::vec2(0) };
            }
        }

        updateSkyline(bestIndex, nextIndex, bestX, bestY, width, height);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_id);
        glTexSubImage2D(GL_TEXTURE_2D, 0, bestX, bestY, width, height, m_format, GL_UNSIGNED_BYTE, data);

        glm::vec2 uv_min(static_cast<float>(bestX) / m_width, static_cast<float>(bestY) / m_height);
        glm::vec2 uv_max(static_cast<float>(bestX + width) / m_width, static_cast<float>(bestY + height) / m_height);
        m_textureCoords[key] = { uv_min, uv_max };

        return { uv_min, uv_max };
    }

    template<typename T>
    concept Printable = requires(T t) {
        { std::cout << t } -> std::same_as<std::ostream&>;
    };

    template<typename T>
    std::pair<glm::vec2, glm::vec2> TextureAtlas<T>::get(const T& key) const {
        auto it = m_textureCoords.find(key);
        if (it != m_textureCoords.end()) {
            return it->second;
        }
        if constexpr (Printable<T>) {
            spdlog::warn("TextureAtlas: Texture \"{}\" not found in atlas.", key);
        } else {
            spdlog::warn("TextureAtlas: Texture not found in atlas.");
        }
        return { glm::vec2(0), glm::vec2(0) };
    }

    template<typename T>
    bool TextureAtlas<T>::expandAtlas() {
        int newWidth = m_width * 2;
        int newHeight = m_height * 2;
        if (newWidth >= s_maxAtlasWidth || newHeight >= s_maxAtlasHeight) {
            spdlog::error("TextureAtlas: Cannot resize atlas, maximum size reached.");
            return false;
        }

        unsigned int newID;
        glGenTextures(1, &newID);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, newID);
        glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, newWidth, newHeight, 0, m_format, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_internalFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_internalFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrapFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrapFilter);

        GLuint srcFBO, dstFBO;
        glGenFramebuffers(1, &srcFBO);
        glGenFramebuffers(1, &dstFBO);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, srcFBO);
        glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_id, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dstFBO);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, newID, 0);

        if (glCheckFramebufferStatus(GL_READ_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            spdlog::error("TextureAtlas: Failed to create read framebuffer for texture atlas.");
            glDeleteTextures(1, &newID);
            glDeleteFramebuffers(1, &srcFBO);
            glDeleteFramebuffers(1, &dstFBO);
            return false;
        }
        if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            spdlog::error("TextureAtlas: Failed to create draw framebuffer for texture atlas.");
            glDeleteTextures(1, &newID);
            glDeleteFramebuffers(1, &srcFBO);
            glDeleteFramebuffers(1, &dstFBO);
            return false;
        }

        glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &srcFBO);
        glDeleteFramebuffers(1, &dstFBO);

        glDeleteTextures(1, &m_id);
        m_id = newID;

        if (m_skyline.back().y != 0)
            m_skyline.emplace_back(m_width, 0);
        
        for (auto& [k, v] : m_textureCoords) {
            v.first *= 0.5f;
            v.second *= 0.5f;
        }
        
        m_width = newWidth;
        m_height = newHeight;
        return true;
    }

    template<typename T>
    bool TextureAtlas<T>::has(const T& key) const {
        return m_textureCoords.contains(key);
    }

    template<typename T>
    void TextureAtlas<T>::use() const {
        if (m_id == 0) {
            spdlog::warn("TextureAtlas: Texture atlas not initialized.");
            return;
        }
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_id);
    }

    template<typename T>
    void TextureAtlas<T>::destroy() {
        m_textureCoords.clear();
        if (m_id != 0) {
            glDeleteTextures(1, &m_id);
            m_id = 0;
        }
        m_width = 0;
        m_height = 0;
        m_initialized = false;
        m_skyline.clear();
    }

    template<typename T>
    void TextureAtlas<T>::init() {
        if (m_initialized) {
            return;
        }

        glGenTextures(1, &m_id);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_id);
        
        glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_width, m_height, 0, m_format, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_internalFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_internalFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrapFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrapFilter);

        m_skyline.emplace_back(0, 0);
        m_initialized = true;
    }

    template<typename T>
    void TextureAtlas<T>::checkMaxTextureSize() {
        if (s_maxAtlasWidth == 0 || s_maxAtlasHeight == 0) {
            glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&s_maxAtlasWidth);
            glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&s_maxAtlasHeight);
        }
    }

    template<typename T>
    bool TextureAtlas<T>::findPosition(int rectWidth, int rectHeight, int &bestIndex, int& nextIndex, int &bestX, int &bestY) const {
        bestY = std::numeric_limits<int>::max();
        bestIndex = -1;
        bestX = 0;

        for (size_t i = 0; i < m_skyline.size(); i++) {
            int x = m_skyline[i].x;
            // Check if the rectangle would exceed the atlas width
            if (x + rectWidth > m_width)
                continue;
            
            // Find the max Y under that fall within the current rectangle's width
            int startX = m_skyline[i].x;
            int endX = startX + rectWidth;
            int maxY = m_skyline[i].y;
            size_t next;
            for (next = i + 1; next < m_skyline.size(); next++) {
                if (m_skyline[next].x >= endX)
                    break;
                maxY = std::max(maxY, m_skyline[next].y);
            }

            // Check if it fits vertically in the atlas
            if (maxY + rectHeight > m_height)
                continue;
            
            // Select the placement with the lowest y position
            if (maxY < bestY) {
                bestY = maxY;
                bestIndex = i;
                nextIndex = next;
                bestX = x;
            }
        }
        return bestIndex != -1;
    }
    
    template<typename T>
    void TextureAtlas<T>::updateSkyline(int bestIndex, int nextIndex, int bestX, int bestY, int rectWidth, int rectHeight) {
        glm::ivec2 topLeft = { bestX, bestY + rectHeight };
        glm::ivec2 bottomRight = { bestX + rectWidth, m_skyline[nextIndex - 1].y };

        bool shouldInsertBottomRight =
            nextIndex < m_skyline.size() ? bottomRight.x < m_skyline[nextIndex].x : bottomRight.x < m_width;
        
        int removeCount = nextIndex - bestIndex;
        int insertCount = 1 + (shouldInsertBottomRight ? 1 : 0);

        m_skyline.insert(m_skyline.begin() + bestIndex, topLeft);
        if (shouldInsertBottomRight) {
            m_skyline.insert(m_skyline.begin() + bestIndex + 1, bottomRight);
        }
        auto removeStart = m_skyline.begin() + bestIndex + insertCount;
        m_skyline.erase(removeStart, removeStart + removeCount);
    }
}