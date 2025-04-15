#include "graphics/gfx/render_target.h"
#include <spdlog/spdlog.h>

namespace gfx
{
    void RenderTarget::setup(int width, int height, unsigned int internalFormat)
    {
        m_width = width;
        m_height = height;
        m_frameBuffer.destroy();
        m_renderBuffer.destroy();
        m_frameBuffer.setup(width, height);
        m_renderBuffer.setup(width, height, internalFormat);
        m_texture = Texture::renderTexture(width, height);

        m_frameBuffer.use();
        m_frameBuffer.attachTexture(m_texture);
        m_frameBuffer.attachRenderBuffer(m_renderBuffer);
        if (!m_frameBuffer.checkFrameBufferStatus())
        {
            spdlog::error("RenderTarget: Framebuffer is not complete.");
            throw std::runtime_error("RenderTarget: Framebuffer is not complete.");
        }
        m_frameBuffer.useDefault();
        m_initialized = true;
    }

    Texture& RenderTarget::getTexture()
    {
        if (!m_initialized)
        {
            spdlog::error("RenderTarget: RenderTarget not initialized. Call setup() first.");
            throw std::runtime_error("RenderTarget: RenderTarget not initialized. Call setup() first.");
        }
        return m_texture;
    }

    void RenderTarget::use() const
    {
        if (m_initialized)
            m_frameBuffer.use();
    }

    void RenderTarget::useDefault()
    {
        FrameBuffer::useDefault();
    }
}