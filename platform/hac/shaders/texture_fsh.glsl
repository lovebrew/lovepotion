#version 460

layout (location = 0) in vec4 inColor;
layout (location = 1) in vec2 inTexCoord;

layout (location = 0) out vec4 outColor;

layout (binding = 0) uniform sampler2D texture0;

void main()
{
    outColor = inColor * texture(texture0, inTexCoord);
}
