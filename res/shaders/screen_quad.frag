#version 330 core

out vec4 outputColor;

in vec2 vTexCoord;

uniform sampler2D uTexture;

void main()
{
    float gamma = 2.2;
    vec4 color = texture(uTexture, vTexCoord);
    outputColor = pow(color, vec4(1.0 / gamma));
}