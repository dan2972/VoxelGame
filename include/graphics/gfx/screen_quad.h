#pragma once

#include <glad/glad.h>
#include <vector>
#include "graphics/gfx/mesh.h"
#include "graphics/gfx/shader.h"
#include "graphics/gfx/texture.h"

namespace gfx
{
    class ScreenQuad
    {
    public:
        ScreenQuad();
        ~ScreenQuad() = default;

        ScreenQuad(const ScreenQuad&) = delete;
        ScreenQuad& operator=(const ScreenQuad&) = delete;
        ScreenQuad(ScreenQuad&&) = default;
        ScreenQuad& operator=(ScreenQuad&&) = default;

        void draw();

    private:
        Mesh m_mesh;
        bool m_initialized = false;

        static std::vector<float> s_vertices;
        static std::vector<unsigned int> s_indices;

        void setup();
    };
}