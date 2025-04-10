#pragma once

#include <vector>
#include "shader.h"
#include "texture.h"

namespace gfx
{
    class Mesh
    {
    public:
        Mesh() = default;
        ~Mesh();

        Mesh(const Mesh& other) = delete;
        Mesh(Mesh&& other) noexcept;
        Mesh& operator=(const Mesh& other) = delete;
        Mesh& operator=(Mesh&& other) noexcept;

        void populate(
            const std::vector<float>& vertices, 
            const std::vector<unsigned int>& indices, 
            const std::vector<unsigned int>& dims,
            GLuint vboHint = GL_STATIC_DRAW, 
            GLuint eboHint = GL_STATIC_DRAW
        );

        void updateVertexBuffer(
            const std::vector<float> &vertices, 
            unsigned int vertCount, 
            bool bindVAO = true
        );

        void updateIndexBuffer(
            const std::vector<unsigned int> &indices, 
            unsigned int indexCount, 
            bool bindVAO = true
        );

        void updateBuffers(
            const std::vector<float> &vertices, 
            const std::vector<unsigned int> &indices, 
            bool bindVAO = true
        );

        void updateBuffers(
            const std::vector<float> &vertices, 
            const std::vector<unsigned int> &indices, 
            unsigned int vertCount, 
            unsigned int indexCount, 
            bool bindVAO = true
        );

        void draw() const;
        void draw(int indexCount) const;

        void destroy();
    private:
        unsigned int m_vao = 0;
        unsigned int m_vbo = 0;
        unsigned int m_ebo = 0;
        
        std::vector<float> m_vertices;
        std::vector<unsigned int> m_indices;

        GLuint m_vboHint = GL_STATIC_DRAW;
        GLuint m_eboHint = GL_STATIC_DRAW;
    };
}