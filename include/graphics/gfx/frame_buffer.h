#pragma once

#include <glad/glad.h>
#include "graphics/gfx/texture.h"
#include "graphics/gfx/render_buffer.h"

namespace gfx
{
    class FrameBuffer
    {
    public:
        FrameBuffer() = default;
        ~FrameBuffer();

        FrameBuffer(const FrameBuffer&) = delete;
        FrameBuffer& operator=(const FrameBuffer&) = delete;
        FrameBuffer(FrameBuffer&&) noexcept;
        FrameBuffer& operator=(FrameBuffer&&) noexcept;

        static void useDefault();
        void setup(int width, int height);
        void use() const;
        void attachTexture(const Texture& texture, unsigned int attachment = GL_COLOR_ATTACHMENT0);
        void attachRenderBuffer(const RenderBuffer& renderBuffer);
        bool checkFrameBufferStatus() const;

        void destroy();

        unsigned int getWidth() const;
        unsigned int getHeight() const;
        unsigned int getID() const;

    private:
        unsigned int m_id = 0;
        unsigned int m_width = 0, m_height = 0;
    };
}