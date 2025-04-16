#include "graphics/gfx/screen_quad.h"

namespace gfx
{
    std::vector<float> ScreenQuad::s_vertices = {
        // positions   // texCoords
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f
    };

    std::vector<unsigned int> ScreenQuad::s_indices =
    {
        0, 1, 2,
        2, 3, 0
    };

    ScreenQuad::ScreenQuad()
    {
    }

    void ScreenQuad::draw()
    {
        if (!m_initialized)
            setup();
        m_mesh.draw();
    }

    void ScreenQuad::setup()
    {
        if (m_initialized)
            return;

        m_mesh.populate(
            s_vertices,
            s_indices,
            {2,2},
            GL_STATIC_DRAW
        );
        m_initialized = true;
    }
}