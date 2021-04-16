#version 460

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec4 inAttrib;

layout (location = 0) out vec4 outAttrib;

layout (std140, binding = 0) uniform Transformation
{
    mat4 mdlvMtx;
    mat4 projMtx;
} u;

void main()
{
    vec4 pos = u.mdlvMtx * vec4(inPos, 1.0);
    gl_Position = u.projMtx * pos;

    outAttrib = inAttrib;
}