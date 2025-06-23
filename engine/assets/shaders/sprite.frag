#version 330 core

in vec4 vColor;
in vec2 vUV;
in float vTextureIndex;

uniform sampler2D uTextures[32]; // bound slots
uniform int uTexSlotMap[128]; // mapping Texture IDs to slots

out vec4 FragColor;

void main()
{
    int slot = uTexSlotMap[int(vTextureIndex)];
    FragColor = texture(uTextures[slot], vUV) * 0.001 + vColor;
}