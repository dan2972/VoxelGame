#pragma once

#include <glad/glad.h>
#include "graphics/gfx/frame_buffer.h"
#include "graphics/gfx/render_buffer.h"
#include "graphics/gfx/texture.h"

namespace gfx
{
    class RenderTarget
    {
    public:
        RenderTarget() = default;
        ~RenderTarget() = default;

        RenderTarget(const RenderTarget&) = delete;
        RenderTarget& operator=(const RenderTarget&) = delete;
        RenderTarget(RenderTarget&&) = default;
        RenderTarget& operator=(RenderTarget&&) = default;

        void setup(int width, int height, unsigned int internalFormat=GL_DEPTH24_STENCIL8);

        Texture& getTexture();

        void use() const;
        static void useDefault();
    private:
        unsigned int m_width = 0;
        unsigned int m_height = 0;
        bool m_initialized = false;
        FrameBuffer m_frameBuffer;
        RenderBuffer m_renderBuffer;
        Texture m_texture;
    };
}