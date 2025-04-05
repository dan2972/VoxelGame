#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec4 vColor;
out vec2 vTexCoord;

uniform vec2 uResolution;

void main(void)
{
    vColor = aColor;
    vTexCoord = aTexCoord;
    vec2 pos = aPosition.xy / uResolution * 2.0 - 1.0;
    gl_Position = vec4(pos.x, -pos.y, 0.0, 1.0);
}