#version 330

out vec4 outputColor;

in vec2 vTexCoord;
in vec3 vNormal;
in float vAOValue;
in float vBlockLightValue;
in float vSunLightValue;

uniform sampler2D uTexture;
uniform bool uAOEnabled;
uniform float uAOIntensity;
uniform float uDayNightFrac;

float diffuse(vec3 normal)
{
    if (abs(normal.y - 1.0) < 0.01)
        return 1.0;
    else if (abs(normal.y + 1.0) < 0.01)
        return 0.7;
    else if (abs(normal.x - 1.0) < 0.01 || abs(normal.x + 1.0) < 0.01)
        return 0.9;
    else if (abs(normal.z - 1.0) < 0.01 || abs(normal.z + 1.0) < 0.01)
        return 0.8;
    else
        return 0.0;
}

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
    // vec3 lightDir = normalize(-vec3(1.0, -1.0, 0.5));
    // float diff = max(dot(vNormal, lightDir), 0.0);
    // float ambient = 0.2;
    float ambientOcclusion = 1;
    if (uAOEnabled)
        ambientOcclusion =  (1 - uAOIntensity) + ((vAOValue / 3.0) * uAOIntensity);

    float sLight = 0.1 + 0.9 * (vSunLightValue * stepPlateau(uDayNightFrac) / 15);
    float bLight = 0.1 + 0.9 * (vBlockLightValue / 15);
    float light = clamp(max(sLight, bLight), 0.0, 1.0);
    float multiplier = light * ambientOcclusion * diffuse(vNormal);
    multiplier = pow(multiplier, 2.2);
    outputColor = vec4(vec3(multiplier), 1.0) * texture(uTexture, vTexCoord);
}