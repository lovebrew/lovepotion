#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec2 VertexTexCoord;
layout (location = 2) in vec4 VertexColor;

layout (location = 0) out vec4 VaryingColor;
layout (location = 1) out vec2 VaryingTexCoord;

layout (std140, binding = 0) uniform Transformation
{
    mat4 mdlvMtx;
    mat4 projMtx;
} u;

void main()
{
    vec4 pos = u.mdlvMtx * vec4(VertexPosition, 1.0);
    gl_Position = u.projMtx * pos;

    VaryingColor = VertexColor;
    VaryingTexCoord = VertexTexCoord;
}
