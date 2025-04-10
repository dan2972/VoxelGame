#pragma once

#include <glad/glad.h>
#include <string>

namespace gfx
{
    class Texture
    {
    public:
        Texture() = default;
        Texture(const std::string& path, GLint internalFormat=GL_SRGB_ALPHA);
        ~Texture();

        Texture(const Texture& other) = delete;
        Texture(Texture&& other) noexcept;
        Texture& operator=(const Texture& other) = delete;
        Texture& operator=(Texture&& other) noexcept;

        void loadFromFile(const std::string& path, GLint internalFormat=GL_SRGB_ALPHA);

        static Texture defaultTexture(GLint internalFormat=GL_SRGB_ALPHA);
        static Texture renderTexture(int width, int height, GLint internalFormat=GL_RGB16F);
        
        void use(GLuint textureUnit=0) const;

        void destroy();
        
        int getWidth();
        int getHeight();
        unsigned int getID();

    private:
        unsigned int m_id = 0;
        int m_width;
        int m_height;
    };
}