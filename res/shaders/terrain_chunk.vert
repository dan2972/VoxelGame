#version 330 core

layout(location = 0) in float aData;
layout(location = 1) in vec2 aTexCoord;

out vec2 vTexCoord;
out vec3 vNormal;
out float vAOValue;
out float vLightValue;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;
uniform vec3 uChunkOffset;

vec3 getNormalFromIndex(int index) {
    switch(int(index))
    {
        case 0: // front
            return vec3(0.0, 0.0, 1.0);
        case 1: // back
            return vec3(0.0, 0.0, -1.0);
        case 2: // left
            return vec3(-1.0, 0.0, 0.0);
        case 3: // right
            return vec3(1.0, 0.0, 0.0);
        case 4: // top
            return vec3(0.0, 1.0, 0.0);
        case 5: // bottom
            return vec3(0.0, -1.0, 0.0);
        default:
            return vec3(0.0, 0.0, 0.0);
    }
}

void main(void)
{
    uint vPacked = floatBitsToUint(aData);
    uint lightLevel = vPacked & 0xFu;        // 4 bits
    vPacked >>= 4;
    uint aoValue = vPacked & 0x3u;            // 2 bits
    vPacked >>= 2;
    uint normalIndex = vPacked & 0x7u;        // 3 bits
    vPacked >>= 3;
    uint posZ = vPacked & 0x1Fu;              // 5 bits
    vPacked >>= 5;
    uint posY = vPacked & 0x1Fu;              // 5 bits
    vPacked >>= 5;
    uint posX = vPacked & 0x1Fu;              // 5 bits
    vPacked >>= 5;

    vec3 aPosition = vec3(float(posX), float(posY), float(posZ));
    vAOValue = float(aoValue);
    vLightValue = float(lightLevel);
    vNormal = getNormalFromIndex(int(normalIndex));
    vTexCoord = aTexCoord;
    gl_Position = uProjection * uView * uModel * vec4(aPosition + uChunkOffset, 1.0);
}