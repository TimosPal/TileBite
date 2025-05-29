// Vertex Shader
#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aUV;
layout (location = 3) in float aTextureIndex;

uniform mat4 uViewProjection;

out vec4 vColor;
out vec2 vUV;
out float vTextureIndex;

void main()
{
    vec4 worldPos = vec4(aPos, 0.0, 1.0);
    gl_Position = uViewProjection * worldPos;
    vColor = aColor;
    vUV = aUV;
    vTextureIndex = aTextureIndex;
}
