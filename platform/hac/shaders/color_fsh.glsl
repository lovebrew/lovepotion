#version 460

layout (location = 0) in  vec4 VertexColor;
layout (location = 0) out vec4 VaryingColor;

void main()
{
    VaryingColor = VertexColor;
}
