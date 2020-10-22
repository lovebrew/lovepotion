#version 460

#define love_Position gl_Position

#define attribute in
#define varying out

#define number float

mediump float gammaToLinearFast(mediump float c) {
    return c * (c * (c * 0.305306011 + 0.682171111) + 0.012522878);
}

mediump vec3 gammaToLinearFast(mediump vec3 c) {
    return c * (c * (c * 0.305306011 + 0.682171111) + 0.012522878);
}

mediump vec4 gammaToLinearFast(mediump vec4 c) {
    return vec4(gammaToLinearFast(c.rgb), c.a);
}

mediump float linearToGammaFast(mediump float c) {
    return max(1.055 * pow(max(c, 0.0), 0.41666666) - 0.055, 0.0);
}

mediump vec3 linearToGammaFast(mediump vec3 c) {
    return max(1.055 * pow(max(c, vec3(0.0)), vec3(0.41666666)) - 0.055, vec3(0.0));
}

mediump vec4 linearToGammaFast(mediump vec4 c) {
    return vec4(linearToGammaFast(c.rgb), c.a);
}

#define gammaToLinear gammaToLinearFast
#define linearToGamma linearToGammaFast

#define gammaCorrectColor gammaToLinear
#define unGammaCorrectColor linearToGamma
#define gammaCorrectColorPrecise gammaToLinearPrecise
#define unGammaCorrectColorPrecise linearToGammaPrecise
#define gammaCorrectColorFast gammaToLinearFast
#define unGammaCorrectColorFast linearToGammaFast

attribute vec4 VertexPosition;
attribute vec4 VertexTexCoord;
attribute vec4 VertexColor;
attribute vec4 ConstantColor;

varying   vec4 VaryingTexCoord;
varying   vec4 VaryingColor;

layout (location = 1) in vec4 inAttrib;

layout (location = 0) out vec4 outAttrib;

layout (std140, binding = 0) uniform Transformation
{
    mat4 mdlvMtx;
    mat4 projMtx;
} u;

void main() {
    VaryingTexCoord = VertexTexCoord;
    VaryingColor = gammaCorrectColor(VertexColor) * ConstantColor;

    vec4 pos = u.mdlvMtx * VertexPosition;
    love_Position = u.projMtx * pos;

    outAttrib = inAttrib;
}