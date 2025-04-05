#version 330 core

layout(location = 0) in vec3 aPosition;

uniform mat4 uProjView;
uniform vec3 uBlockPos;

void main(void)
{
    gl_Position = uProjView * vec4(aPosition + uBlockPos, 1.0);
}