#pragma once

#include <glad/glad.h>

namespace gfx
{
    class RenderBuffer
    {
    public:
        RenderBuffer() = default;
        ~RenderBuffer();

        RenderBuffer(const RenderBuffer&) = delete;
        RenderBuffer& operator=(const RenderBuffer&) = delete;
        RenderBuffer(RenderBuffer&&) noexcept;
        RenderBuffer& operator=(RenderBuffer&&) noexcept;

        void setup(int width, int height, unsigned int internalFormat=GL_DEPTH24_STENCIL8);
        void use() const;
        void destroy();

        unsigned int getWidth() const { return m_width; }
        unsigned int getHeight() const { return m_height; }
        unsigned int getID() const { return m_id; }

    private:
        unsigned int m_id = 0;
        unsigned int m_width = 0, m_height = 0;
    };
}