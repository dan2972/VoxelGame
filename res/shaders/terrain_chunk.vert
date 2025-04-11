#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in float aNormalIndex;
layout(location = 3) in float aAOValue;
layout(location = 4) in float aLightValue;

out vec2 vTexCoord;
out vec3 vNormal;
out float vAOValue;
out float vLightValue;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;
uniform vec3 uChunkOffset;

vec3 getNormalFromIndex(int index) {
    switch(int(aNormalIndex))
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
    gl_Position = uProjection * uView * uModel * vec4(aPosition + uChunkOffset, 1.0);
    vTexCoord = aTexCoord;
    vNormal = getNormalFromIndex(int(aNormalIndex));
    vAOValue = aAOValue;
    vLightValue = aLightValue;
}