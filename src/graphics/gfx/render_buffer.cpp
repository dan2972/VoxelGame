#include "graphics/gfx/render_buffer.h"

namespace gfx
{
    RenderBuffer::~RenderBuffer()
    {
        destroy();
    }

    RenderBuffer::RenderBuffer(RenderBuffer&& other) noexcept
    {
        destroy();
        m_id = other.m_id;
        m_width = other.m_width;
        m_height = other.m_height;
        other.m_id = 0;
        other.m_width = 0;
        other.m_height = 0;
    }

    RenderBuffer& RenderBuffer::operator=(RenderBuffer&& other) noexcept
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

    void RenderBuffer::setup(int width, int height, unsigned int internalFormat)
    {
        destroy();
        m_width = width;
        m_height = height;
        glGenRenderbuffers(1, &m_id);
        glBindRenderbuffer(GL_RENDERBUFFER, m_id);
        glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    void RenderBuffer::use() const
    {
        glBindRenderbuffer(GL_RENDERBUFFER, m_id);
    }

    void RenderBuffer::destroy()
    {
        if (m_id != 0)
        {
            glDeleteRenderbuffers(1, &m_id);
            m_id = 0;
            m_width = 0;
            m_height = 0;
        }
    }
}