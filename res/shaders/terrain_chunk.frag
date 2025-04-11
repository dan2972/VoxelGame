#version 330

out vec4 outputColor;

in vec2 vTexCoord;
in vec3 vNormal;
in float vAOValue;
in float vLightValue;

uniform sampler2D uTexture;
uniform bool uAOEnabled;
uniform float uAOIntensity;

void main()
{
    vec3 lightDir = normalize(-vec3(1.0, -1.0, 0.5));
    float diff = max(dot(vNormal, lightDir), 0.0);
    float ambient = 0.2;
    float ambientOcclusion = 1;
    if (uAOEnabled)
        ambientOcclusion =  (1 - uAOIntensity) + ((vAOValue / 3.0) * uAOIntensity);

    float light = 0.1 + 0.9 * (vLightValue / 15);
    light = clamp(light, 0.0, 1.0);
    float multiplier = light * ambientOcclusion * (ambient + diff);
    outputColor = vec4(vec3(multiplier), 1.0) * texture(uTexture, vTexCoord);
}