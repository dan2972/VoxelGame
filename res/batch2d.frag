#version 330

out vec4 outputColor;

in vec4 vColor;
in vec2 vTexCoord;

uniform sampler2D uTexture;

void main()
{
    outputColor = texture(uTexture, vTexCoord) * vec4(vColor);
}