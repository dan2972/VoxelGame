#include "graphics/line_renderer.h"

#include <glad/glad.h>

namespace gfx
{
    LineRenderer::LineRenderer()
    {
    }
    
    LineRenderer::~LineRenderer()
    {
    }

    LineRenderer& LineRenderer::operator=(LineRenderer&& other) noexcept
    {
        if (this != &other)
        {
            m_vertices = std::move(other.m_vertices);
            m_indices = std::move(other.m_indices);
            m_curVertex = other.m_curVertex;
            m_curIndex = other.m_curIndex;
            m_meshInitialized = other.m_meshInitialized;
            m_mesh = std::move(other.m_mesh);
            other.m_curVertex = 0;
            other.m_curIndex = 0;
            other.m_meshInitialized = false;
        }
        return *this;
    }

    LineRenderer::LineRenderer(LineRenderer&& other) noexcept
        : m_vertices(std::move(other.m_vertices)),
          m_indices(std::move(other.m_indices)),
          m_curVertex(other.m_curVertex),
          m_curIndex(other.m_curIndex),
          m_meshInitialized(other.m_meshInitialized),
          m_mesh(std::move(other.m_mesh))
    {
        other.m_curVertex = 0;
        other.m_curIndex = 0;
        other.m_meshInitialized = false;
    }

    void LineRenderer::beginBatch()
    {
        if (!m_meshInitialized)
        {
            m_mesh.populate({}, {}, {3, 3, 4, 1}, GL_DYNAMIC_DRAW, GL_DYNAMIC_DRAW);
            m_meshInitialized = true;
        }
        m_curVertex = 0;
        m_curIndex = 0;
    }

    void LineRenderer::drawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color)
    {
        if (m_curVertex + 4 >= m_vertices.size())
            m_vertices.resize(m_curVertex + 4);
        if (m_curIndex + 6 >= m_indices.size())
            m_indices.resize(m_curIndex + 6);

        m_vertices[m_curVertex + 0] = { start, end, color, 1.0f };
        m_vertices[m_curVertex + 1] = { end, start, color, -1.0f };
        m_vertices[m_curVertex + 2] = { end, start, color, 1.0f };
        m_vertices[m_curVertex + 3] = { start, end, color, -1.0f };

        m_indices[m_curIndex + 0] = m_curVertex + 0;
        m_indices[m_curIndex + 1] = m_curVertex + 1;
        m_indices[m_curIndex + 2] = m_curVertex + 2;
        m_indices[m_curIndex + 3] = m_curVertex + 0;
        m_indices[m_curIndex + 4] = m_curVertex + 2;
        m_indices[m_curIndex + 5] = m_curVertex + 3;

        m_curVertex += 4;
        m_curIndex += 6;
    }

    void LineRenderer::draw(bool bindVAO)
    {
        if (m_curVertex == 0)
            return;

        float* verticesFloats = reinterpret_cast<float*>(m_vertices.data());
        size_t verticesSize = m_curVertex * sizeof(Vertex) / sizeof(float);
        auto verticesVec = std::vector<float>(verticesFloats, verticesFloats + verticesSize);
        m_mesh.updateVertexBuffer(verticesVec, verticesSize, bindVAO);
        m_mesh.updateIndexBuffer(m_indices, m_curIndex, false);
        m_mesh.draw(m_curIndex);
    }


}