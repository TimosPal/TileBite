// Fragment Shader
#version 330 core

in vec4 vColor;
in vec2 vUV;

uniform sampler2D uTexture;

out vec4 FragColor;

void main()
{
    vec4 texColor = texture(uTexture, vUV);
    vec3 tinted = mix(texColor.rgb, texColor.rgb * vColor.rgb, vColor.a);
    FragColor = vec4(tinted, texColor.a);
}
