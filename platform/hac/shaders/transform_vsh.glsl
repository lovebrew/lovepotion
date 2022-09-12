#version 460

layout (location = 0) in vec3 inPos;

// in attributes -- nothing happens if not used
layout (location = 1) in vec4 inColor;
layout (location = 2) in vec2 inTexCoord;

// out attributes -- nothing happens if not used
layout (location = 0) out vec4 outColor;
layout (location = 1) out vec2 outTexCoord;

layout (std140, binding = 0) uniform Transformation
{
    mat4 mdlvMtx;
    mat4 projMtx;
} u;

void main()
{
    vec4 pos = u.mdlvMtx * vec4(inPos, 1.0);
    gl_Position = u.projMtx * pos;

    outColor = inColor;
    outTexCoord = inTexCoord;
}
