#version 330 core

in vec4 vColor;
in vec2 vUV;
in float vTextureIndex;

uniform sampler2D uTextures[32];

out vec4 FragColor;

void main()
{
    int index = int(vTextureIndex);
    FragColor = texture(uTextures[index], vUV) * vColor;
}
