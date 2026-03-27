#version 460

layout (location = 0) in vec4 VertexColor;
layout (location = 1) in vec2 VertexTexCoord;

layout (location = 0) out vec4 VaryingColor;

layout (binding = 0) uniform sampler2D texture0;

void main()
{
    VaryingColor = VertexColor * texture(texture0, VertexTexCoord);
}
