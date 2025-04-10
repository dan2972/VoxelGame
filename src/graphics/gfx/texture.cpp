#include "graphics/gfx/texture.h"
#include <spdlog/spdlog.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace gfx
{
    Texture::Texture(const std::string &path, GLint internalFormat)
    {
        loadFromFile(path, internalFormat);
    }

    Texture::~Texture()
    {
        destroy();
    }

    Texture &Texture::operator=(Texture &&other) noexcept
    {
        if (this != &other)
        {
            destroy();
            m_id = other.m_id;
            m_width = other.m_width;
            m_height = other.m_height;
            other.m_id = 0;
        }
        return *this;
    }

    Texture::Texture(Texture &&other) noexcept
    {
        destroy();
        m_id = other.m_id;
        m_width = other.m_width;
        m_height = other.m_height;
        other.m_width = 0;
        other.m_height = 0;
        other.m_id = 0;
    }

    void Texture::loadFromFile(const std::string &path, GLint internalFormat)
    {
        GLuint id; 
        int width, height, channels;
        glGenTextures(1, &id);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, id);

        stbi_set_flip_vertically_on_load(true);
        
        unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 4);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glGenerateMipmap(GL_TEXTURE_2D);

            stbi_image_free(data);
        }
        else
        {
            spdlog::error("Failed to load texture: {}", path);
        }

        m_id = id;
        m_width = width;
        m_height = height;
    }

    Texture Texture::defaultTexture(GLint internalFormat)
    {
        static GLuint id;
        static bool initialized = false;
        if (initialized) {
            Texture texture;
            texture.m_id = id;
            texture.m_width = 1;
            texture.m_height = 1;
            return texture;
        }
        
        glGenTextures(1, &id);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        unsigned char data[] = {255, 255, 255, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        initialized = true;

        Texture texture;
        texture.m_id = id;
        texture.m_width = 1;
        texture.m_height = 1;
        return texture;
    }

    Texture Texture::renderTexture(int width, int height, GLint internalFormat)
    {
        unsigned int id;
        glGenTextures(1, &id);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

        Texture texture;
        texture.m_id = id;
        texture.m_width = width;
        texture.m_height = height;
        return texture;
    }

    void Texture::use(GLuint textureUnit) const
    {
        if (m_id == 0)
        {
            spdlog::warn("Texture not loaded, cannot use texture.");
            return;
        }
        glActiveTexture(textureUnit);
        glBindTexture(GL_TEXTURE_2D, m_id);
    }

    void Texture::destroy()
    {
        if (m_id)
            glDeleteTextures(1, &m_id);
        m_id = 0;
    }

    int Texture::getWidth()
    {
        return m_width;
    }

    int Texture::getHeight()
    {
        return m_height;
    }

    unsigned int Texture::getID()
    {
        return m_id;
    }
}