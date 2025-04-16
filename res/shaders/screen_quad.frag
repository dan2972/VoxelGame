#version 330 core

out vec4 outputColor;

in vec2 vTexCoord;

uniform vec2 uResolution;
uniform bool uShowCrosshair;

uniform sampler2D uTexture;

bool curPixelInCrossHair(float lineWidth = 1.0, float size = 10.0) {
    float padding = 0;
    if (int(uResolution.y) % 2 != 0)
        padding = 0.5;
    return (abs(gl_FragCoord.x - uResolution.x / 2.0) < size && 
            abs(gl_FragCoord.y - (uResolution.y - padding) / 2.0) < lineWidth) ||
            (abs(gl_FragCoord.x - uResolution.x / 2.0) < lineWidth &&
            abs(gl_FragCoord.y - uResolution.y / 2.0) < size);
}

void main()
{
    float gamma = 2.2;
    vec4 color = texture(uTexture, vTexCoord);
    color = pow(color, vec4(1.0 / gamma));
    if (curPixelInCrossHair() && uShowCrosshair) {
        color = vec4(1 - color.xyz, 1);
    }
    outputColor = color;
}