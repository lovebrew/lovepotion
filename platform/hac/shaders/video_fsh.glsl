#version 460

layout (location = 0) in vec4 inColor;
layout (location = 1) in vec2 inTexCoord;

layout (location = 0) out vec4 outColor;

layout (binding = 0) uniform sampler2D video_y;
layout (binding = 1) uniform sampler2D video_cb;
layout (binding = 2) uniform sampler2D video_cr;

const vec3 yuv_add = vec3(-0.0627451017, -0.501960814, -0.501960814);

const vec3 yuv_r = vec3(1.164,  0.000,  1.596);
const vec3 yuv_g = vec3(1.164, -0.391, -0.813);
const vec3 yuv_b = vec3(1.164,  2.018,  0.000);

void main()
{
    vec3 yuv;

    yuv[0] = texture(video_y,  inTexCoord).r;
    yuv[1] = texture(video_cb, inTexCoord).r;
    yuv[2] = texture(video_cr, inTexCoord).r;

    yuv += yuv_add;

    outColor[0] = dot(yuv, yuv_r);
    outColor[1] = dot(yuv, yuv_g);
    outColor[2] = dot(yuv, yuv_b);
    outColor[3] = 1.0;
}
