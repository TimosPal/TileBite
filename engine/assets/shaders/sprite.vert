// Vertex Shader
#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aUV;
layout (location = 3) in float aTextureIndex;

out vec4 vColor;
out vec2 vUV;
out float vTextureIndex;

void main()
{
    gl_Position = vec4(aPos, 0.0, 1.0);
    vColor = aColor;
    vUV = aUV;
    vTextureIndex = aTextureIndex;
}
