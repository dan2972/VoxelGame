#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aCenter;
layout(location = 2) in vec4 aColor;
layout (location = 3) in vec2 aTexCoord;

out vec4 vColor;
out vec2 vTexCoord;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

void main(void)
{
    vColor = aColor;
    vTexCoord = aTexCoord;

    vec3 cameraRight = vec3(uView[0][0], uView[1][0], uView[2][0]);
    vec3 cameraUp    = vec3(uView[0][1], uView[1][1], uView[2][1]);
    
    vec3 worldPos = aCenter + aPosition.x * cameraRight + aPosition.y * cameraUp;

    gl_Position = uProjection * uView * uModel * vec4(worldPos, 1.0);
}