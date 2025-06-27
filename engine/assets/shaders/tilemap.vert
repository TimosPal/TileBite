// Vertex Shader
#version 330 core

layout (location = 0) in uint aPackedXYIndexUV;
layout (location = 1) in uint aPackedRGBA;

uniform mat4 uViewProjection;
uniform float uWorldTileSize;
uniform float uTextureTileSize;

uniform vec2 offset;

out vec4 vColor;
out vec2 vUV;
out float vTextureIndex;

vec4 unpackRGBA(uint pack)
{
    return vec4(
        float((pack >> 0u) & 0xFFu),
        float((pack >> 8u) & 0xFFu),
        float((pack >> 16u) & 0xFFu),
        float((pack >> 24u) & 0xFFu)
    ) / 255.0;
}

vec2 unpackPos(uint pack)
{
    return vec2(
        float((pack >> 0u) & 0xFFu),   // X index
        float((pack >> 8u) & 0xFFu)    // Y index
    );
}

vec2 unpackUV(uint pack)
{
    return vec2(
        float((pack >> 16u) & 0xFFu),  // UV.x
        float((pack >> 24u) & 0xFFu)   // UV.y
    );
}

void main()
{
    vec2 tileIndex = unpackPos(aPackedXYIndexUV);     // Tilemap X,Y index
    vec2 uvIndex = unpackUV(aPackedXYIndexUV);        // Tile UV index in atlas

    // World position in tilemap
    vec2 worldPosXY = tileIndex * float(uWorldTileSize) + offset;
    vec4 worldPos = vec4(worldPosXY, 0.0, 1.0);
    gl_Position = uViewProjection * worldPos;

    // Convert tile UV index to normalized texture coordinates
    vec2 atlasSize = vec2(uTextureTileSize) * 256.0;   // 256 max tile count (from 8-bit packing)
    vUV = uvIndex * float(uTextureTileSize) / atlasSize;

    vColor = unpackRGBA(aPackedRGBA);
    vTextureIndex = 0.0; // Using a single atlas
}