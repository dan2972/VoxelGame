#include "graphics/gfx/line_renderer.h"

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
        m_vertices[m_curVertex + 1] = { start, end, color, -1.0f };
        m_vertices[m_curVertex + 2] = { end, start, color, 1.0f };
        m_vertices[m_curVertex + 3] = { end, start, color, -1.0f };

        m_indices[m_curIndex + 0] = m_curVertex + 0;
        m_indices[m_curIndex + 1] = m_curVertex + 1;
        m_indices[m_curIndex + 2] = m_curVertex + 2;
        m_indices[m_curIndex + 3] = m_curVertex + 0;
        m_indices[m_curIndex + 4] = m_curVertex + 2;
        m_indices[m_curIndex + 5] = m_curVertex + 3;

        m_curVertex += 4;
        m_curIndex += 6;
    }

    void LineRenderer::drawCube(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color)
    {
        glm::vec3 halfSize = size * 0.5f;
        glm::vec3 vertices[8] = {
            { -halfSize.x, -halfSize.y, -halfSize.z },
            { halfSize.x, -halfSize.y, -halfSize.z },
            { halfSize.x, halfSize.y, -halfSize.z },
            { -halfSize.x, halfSize.y, -halfSize.z },
            { -halfSize.x, -halfSize.y, halfSize.z },
            { halfSize.x, -halfSize.y, halfSize.z },
            { halfSize.x, halfSize.y, halfSize.z },
            { -halfSize.x, halfSize.y, halfSize.z }
        };

        for (int i = 0; i < 8; ++i)
        {
            vertices[i] += position;
        }

        drawLine(vertices[0], vertices[1], color);
        drawLine(vertices[1], vertices[2], color);
        drawLine(vertices[2], vertices[3], color);
        drawLine(vertices[3], vertices[0], color);
        drawLine(vertices[4], vertices[5], color);
        drawLine(vertices[5], vertices[6], color);
        drawLine(vertices[6], vertices[7], color);
        drawLine(vertices[7], vertices[4], color);
        drawLine(vertices[0], vertices[4], color);
        drawLine(vertices[1], vertices[5], color);
        drawLine(vertices[2], vertices[6], color);
        drawLine(vertices[3], vertices[7], color);
    }

    // Draw an axis-aligned 2D grid on a plane defined by two corners
    void LineRenderer::drawAA2DGrid(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color, int segments, bool drawBorders)
    {
        glm::vec3 direction = end - start;
        glm::vec3 step = direction / static_cast<float>(segments);
        for (int i = 0; i <= segments; ++i)
        {
            if (!drawBorders && i == 0 || i == segments)
                continue;
            glm::vec3 point = start + step * static_cast<float>(i);

            glm::vec3 absDir = glm::abs(direction);
            if (absDir.x <= 1e-6f || absDir.z <= 1e-6f) {
                drawLine({start.x, point.y, start.z}, {end.x, point.y, end.z}, color);
            }
            if (absDir.y <= 1e-6f || absDir.z <= 1e-6f) {
                drawLine({point.x, start.y, start.z}, {point.x, end.y, end.z}, color);
            }
            if (absDir.x <= 1e-6f || absDir.y <= 1e-6f) {
                drawLine({start.x, start.y, point.z}, {end.x, end.y, point.z}, color);
            }
        }
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