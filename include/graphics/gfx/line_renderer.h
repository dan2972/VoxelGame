#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "mesh.h"

namespace gfx
{
    class LineRenderer
    {
    public:
        LineRenderer();
        ~LineRenderer();

        LineRenderer(const LineRenderer& other) = delete;
        LineRenderer& operator=(const LineRenderer& other) = delete;
        LineRenderer(LineRenderer&& other) noexcept;
        LineRenderer& operator=(LineRenderer&& other) noexcept;

        void beginBatch();
        void drawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color);
        void drawCube(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color);
        void drawAA2DGrid(const glm::vec3& corner1, const glm::vec3& corner2, const glm::vec4& color, int segments, bool drawBorders=true);
        void draw(bool bindVAO = true);

    private:
        struct Vertex
        {
            glm::vec3 position;
            glm::vec3 other;
            glm::vec4 color;
            float side;
        };

        std::vector<Vertex> m_vertices;
        std::vector<unsigned int> m_indices;
        unsigned int m_curVertex = 0;
        unsigned int m_curIndex = 0;

        Mesh m_mesh;
        bool m_meshInitialized = false;
    };
}