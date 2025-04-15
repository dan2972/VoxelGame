#include "graphics/gfx/frame_buffer.h"

namespace gfx
{
    FrameBuffer::~FrameBuffer()
    {
        destroy();
    }

    FrameBuffer::FrameBuffer(FrameBuffer&& other) noexcept
    {
        destroy();
        m_id = other.m_id;
        m_width = other.m_width;
        m_height = other.m_height;
        other.m_id = 0;
        other.m_width = 0;
        other.m_height = 0;
    }

    FrameBuffer& FrameBuffer::operator=(FrameBuffer&& other) noexcept
    {
        if (this != &other)
        {
            destroy();
            m_id = other.m_id;
            m_width = other.m_width;
            m_height = other.m_height;
            other.m_id = 0;
            other.m_width = 0;
            other.m_height = 0;
        }
        return *this;
    }

    void FrameBuffer::useDefault()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBuffer::setup(int width, int height)
    {
        destroy();
        m_width = width;
        m_height = height;
        glGenFramebuffers(1, &m_id);
    }

    void FrameBuffer::use() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_id);
    }

    void FrameBuffer::attachTexture(const Texture& texture, unsigned int attachment)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture.getID(), 0);
    }

    void FrameBuffer::attachRenderBuffer(const RenderBuffer& renderBuffer)
    {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer.getID());
    }

    bool FrameBuffer::checkFrameBufferStatus() const
    {
        return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    }

    void FrameBuffer::destroy()
    {
        if (m_id)
            glDeleteFramebuffers(1, &m_id);
        m_id = 0;
    }

    unsigned int FrameBuffer::getWidth() const
    {
        return m_width;
    }

    unsigned int FrameBuffer::getHeight() const
    {
        return m_height;
    }

    unsigned int FrameBuffer::getID() const
    {
        return m_id;
    }
}