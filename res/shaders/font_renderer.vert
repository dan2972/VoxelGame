#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec4 vColor;
out vec2 vTexCoord;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

void main(void)
{
    vColor = aColor;
    vTexCoord = aTexCoord;
    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
}