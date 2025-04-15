#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec4 aColor;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;
uniform float uLineWidth;
uniform vec2 uResolution;

out vec4 vColor;

void main() {
    vColor = aColor;

    vec4 lineStart = uProjection * uView * uModel * vec4(aPos, 1.0);
    vec4 lineEnd = uProjection * uView * uModel * vec4(aPos + aNormal, 1.0);

    vec3 ndcStart = lineStart.xyz / lineStart.w;
    vec3 ndcEnd = lineEnd.xyz / lineEnd.w;

    vec2 ndcDir = normalize((ndcEnd.xy - ndcStart.xy) * uResolution);
    vec2 ndcOffset = vec2(-ndcDir.y, ndcDir.x) * uLineWidth / uResolution;

    if (ndcOffset.x < 0.0) {
        ndcOffset = -ndcOffset;
    }

    if (gl_VertexID % 2 == 0) {
        gl_Position = vec4((ndcStart + vec3(ndcOffset, 0.0)) * lineStart.w, lineStart.w);
    } else {
        gl_Position = vec4((ndcStart - vec3(ndcOffset, 0.0)) * lineStart.w, lineStart.w);
    }
}