#version 330

out vec4 outputColor;

in vec4 vColor;
in vec2 vTexCoord;

uniform sampler2D uTexture;

void main()
{
    outputColor = vec4(1.0, 1.0, 1.0, texture(uTexture, vTexCoord).r) * vec4(vColor);
}