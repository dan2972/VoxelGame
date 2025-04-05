#version 330 core

out vec4 outputColor;

in vec2 vTexCoord;

uniform mat4 uInvViewProj;

void main()
{
    vec4 clipPos = vec4(2.0 * vTexCoord - 1.0, 1.0, 1.0);
    vec4 worldPos = uInvViewProj * clipPos;
    vec3 worldDir = normalize(worldPos.xyz / worldPos.w);

    float t = 0.5 * (worldDir.y + 1.0);
    vec3 skyColor = mix(vec3(1.0, 1.0, 1.0), vec3(0.5, 0.7, 1.0), t);

    outputColor = vec4(skyColor, 1.0);
}