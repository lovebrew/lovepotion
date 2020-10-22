#version 460

#define love_MaxCanvases gl_MaxDrawBuffers
#define varying in

#ifdef LOVE_MULTI_CANVAS
    layout(location = 0) out vec4 love_Canvases[love_MaxCanvases];
    #define love_PixelColor love_Canvases[0]
#else
    layout(location = 0) out vec4 love_PixelColor;
#endif

#define love_PixelCoord (vec2(gl_FragCoord.x, (gl_FragCoord.y * love_ScreenSize.z) + love_ScreenSize.w))

uniform sampler2D MainTex;

// void effect();

void main() {
    // effect();
}