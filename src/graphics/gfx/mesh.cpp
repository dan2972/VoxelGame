#include "graphics/gfx/mesh.h"
#include <numeric>
#include <spdlog/spdlog.h>

namespace gfx
{
    Mesh::~Mesh()
    {
        destroy();
    }

    Mesh::Mesh(Mesh&& other) noexcept
    {
        destroy();
        m_vao = other.m_vao;
        m_vbo = other.m_vbo;
        m_ebo = other.m_ebo;
        m_vertices = std::move(other.m_vertices);
        m_indices = std::move(other.m_indices);
        m_vboHint = other.m_vboHint;
        m_eboHint = other.m_eboHint;
        
        other.m_vao = 0;
        other.m_vbo = 0;
        other.m_ebo = 0;
    }

    Mesh& Mesh::operator=(Mesh&& other) noexcept
    {
        if (this != &other)
        {
            destroy();
            m_vao = other.m_vao;
            m_vbo = other.m_vbo;
            m_ebo = other.m_ebo;
            m_vertices = std::move(other.m_vertices);
            m_indices = std::move(other.m_indices);
            m_vboHint = other.m_vboHint;
            m_eboHint = other.m_eboHint;

            other.m_vao = 0;
            other.m_vbo = 0;
            other.m_ebo = 0;
        }
        return *this;
    }

    void Mesh::populate(
        const std::vector<float>& vertices, 
        const std::vector<unsigned int>& indices, 
        const std::vector<unsigned int>& dims,
        GLuint vboHint, 
        GLuint eboHint
    )
    {
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        
        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(
            GL_ARRAY_BUFFER, 
            vertices.size() * sizeof(GL_FLOAT), 
            vertices.data(), 
            vboHint
        );

        int dimSum = std::accumulate(dims.begin(), dims.end(), 0);
        int dimAccumulator = 0;
        for (int i = 0; i < dims.size(); i++)
        {
            glVertexAttribPointer(
                i, 
                dims[i], 
                GL_FLOAT, 
                GL_FALSE, 
                dimSum * sizeof(GL_FLOAT), 
                (void*)(dimAccumulator * sizeof(GL_FLOAT))
            );
            glEnableVertexAttribArray(i);
            dimAccumulator += dims[i];
        }

        glGenBuffers(1, &m_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER, 
            indices.size() * sizeof(GLuint), 
            indices.data(), 
            eboHint
        );

        m_indices = indices;
        m_vertices = vertices;
        m_vboHint = vboHint;
        m_eboHint = eboHint;
    }

    void Mesh::updateVertexBuffer(const std::vector<float> &vertices, unsigned int vertCount, bool bindVAO)
    {
        if (bindVAO)
            glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        if (vertCount > this->m_vertices.size())
        {
            glBufferData(
                GL_ARRAY_BUFFER, 
                vertices.size() * sizeof(GL_FLOAT), 
                vertices.data(), 
                m_vboHint
            );
        }
        else
        {
            glBufferSubData(
                GL_ARRAY_BUFFER, 
                0, 
                vertices.size() * sizeof(GL_FLOAT), 
                vertices.data()
            );
        }

        this->m_vertices = vertices;
    }

    void Mesh::updateIndexBuffer(const std::vector<unsigned int> &indices, unsigned int indexCount, bool bindVAO)
    {
        if (bindVAO)
            glBindVertexArray(m_vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        if (indexCount > this->m_indices.size())
        {
            glBufferData(
                GL_ELEMENT_ARRAY_BUFFER, 
                indices.size() * sizeof(GLuint), 
                indices.data(), 
                m_eboHint
            );
        }
        else
        {
            glBufferSubData(
                GL_ELEMENT_ARRAY_BUFFER, 
                0, 
                indices.size() * sizeof(GLuint), 
                indices.data()
            );
        }

        this->m_indices = indices;
    }

    void Mesh::updateBuffers(
        const std::vector<float> &vertices, 
        const std::vector<unsigned int> &indices, 
        bool bindVAO
    )
    {
        updateVertexBuffer(vertices, vertices.size(), bindVAO);
        updateIndexBuffer(indices, indices.size(), bindVAO);
    }

    void Mesh::updateBuffers(
        const std::vector<float> &vertices, 
        const std::vector<unsigned int> &indices, 
        unsigned int vertCount, 
        unsigned int indexCount, 
        bool bindVAO
    )
    {
        updateVertexBuffer(vertices, vertCount, bindVAO);
        updateIndexBuffer(indices, indexCount, bindVAO);
    }

    void Mesh::draw() const
    {
        draw(m_indices.size());
    }

    void Mesh::draw(int indexCount) const
    {
        if (m_vao == 0)
        {
            spdlog::warn("Mesh not populated, cannot draw mesh.");
            return;
        }

        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    }

    void Mesh::destroy()
    {
        if (m_vao)
            glDeleteVertexArrays(1, &m_vao);
        if (m_vbo)
            glDeleteBuffers(1, &m_vbo);
        if (m_ebo)
            glDeleteBuffers(1, &m_ebo);
        m_vao = 0;
        m_vbo = 0;
        m_ebo = 0;
    }
}