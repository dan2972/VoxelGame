#version 330 core

out vec4 outputColor;

in vec2 vTexCoord;

uniform mat4 uInvViewProj;
uniform float uDayNightFrac;

vec3 nightSkyColorTop = vec3(0.067, 0.067, 0.18);
vec3 nightSkyColorBottom = vec3(0.1, 0.1, 0.31);
vec3 daySkyColorTop = vec3(0.5, 0.7, 1.0);
vec3 daySkyColorBottom = vec3(1.0, 1.0, 1.0);

float stepPlateau(float x) {
    float k = 40.0;
    float a = 0.25;
    float b = 0.75;
    float rise = 1.0 / (1.0 + exp(-k * (x - a)));
    float fall = 1.0 / (1.0 + exp(-k * (x - b)));
    return rise * (1.0 - fall);
}

void main()
{
    vec4 clipPos = vec4(2.0 * vTexCoord - 1.0, 1.0, 1.0);
    vec4 worldPos = uInvViewProj * clipPos;
    vec3 worldDir = normalize(worldPos.xyz / worldPos.w);

    float t = 0.5 * (worldDir.y + 1.0);
    vec3 skyColor = mix(mix(nightSkyColorBottom, nightSkyColorTop, t), mix(daySkyColorBottom, daySkyColorTop, t), stepPlateau(uDayNightFrac));

    skyColor = pow(skyColor, vec3(2.2)); // Gamma correction
    outputColor = vec4(skyColor, 1.0);
}