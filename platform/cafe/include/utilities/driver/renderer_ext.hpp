#pragma once

#include <utilities/driver/renderer/renderer.tcc>

#include <common/color.hpp>
#include <common/math.hpp>
#include <common/pixelformat.hpp>

#include <modules/graphics_ext.hpp>

#include <utilities/driver/renderer/renderstate.hpp>
#include <utilities/driver/renderer/samplerstate.hpp>
#include <utilities/driver/renderer/vertex.hpp>

#include <gx2/clear.h>
#include <gx2/display.h>
#include <gx2/draw.h>
#include <gx2/event.h>
#include <gx2/mem.h>
#include <gx2/registers.h>
#include <gx2/state.h>
#include <gx2/swap.h>
#include <gx2/utils.h>

#include <gx2r/buffer.h>
#include <gx2r/draw.h>

#include <whb/gfx.h>

/* Enforces GLSL std140/std430 alignment rules for glm types */
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
/* Enables usage of SIMD CPU instructions (requiring the above as well) */
#define GLM_FORCE_INTRINSICS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <array>
#include <functional>

namespace love
{
    enum class Screen : uint8_t
    {
        SCREEN_TV,
        SCREEN_GAMEPAD
    };

    template<>
    class Renderer<Console::CAFE> : public Renderer<Console::ALL>
    {
      private:
        static constexpr const char* RENDERER_NAME    = "GX2";
        static constexpr const char* RENDERER_VERSION = "1.0.0";
        static constexpr const char* RENDERER_VENDOR  = "AMD";
        static constexpr const char* RENDERER_DEVICE  = "Latte";

        static constexpr uint8_t MAX_RENDERTARGETS = 0x02;

        static constexpr auto FRAMEBUFFER_FORMAT    = GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8;
        static constexpr auto FRAMEBUFFER_BUFFERING = GX2_BUFFERING_MODE_DOUBLE;
        static constexpr auto INVALIDATE_COLOR_BUFFER =
            GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_COLOR_BUFFER;

        Renderer();

      public:
        static Renderer& Instance()
        {
            static Renderer instance;
            return instance;
        }

        ~Renderer();

        Info GetRendererInfo();

        void DestroyFramebuffers();

        void CreateFramebuffers();

        void Clear(const Color& color);

        void ClearDepthStencil(int stencil, uint8_t mask, double depth);

        void SetBlendColor(const Color& color);

        void SetBlendMode(const RenderState::BlendState& state);

        void EnsureInFrame();

        /* todo: canvases */
        void BindFramebuffer(Texture<Console::CAFE>* texture = nullptr);

        void Present();

        void SetViewport(const Rect& viewport);

        void SetScissor(const Rect& scissor, bool canvasActive);

        void SetStencil(RenderState::CompareMode mode, int value);

        void SetMeshCullMode(vertex::CullMode mode);

        void SetVertexWinding(vertex::Winding winding);

        void SetSamplerState(Texture<Console::CAFE>* texture, SamplerState& state);

        void SetColorMask(const RenderState::ColorMask& mask);

        void SetLineWidth(float lineWidth);

        void SetLineStyle(Graphics<>::LineStyle style);

        void SetPointSize(float size);

        bool Render(Graphics<Console::CAFE>::DrawCommand& command);

        void UseProgram(const WHBGfxShaderGroup& group);

        std::optional<Screen> CheckScreen(const char* name) const;

        SmallTrivialVector<const char*, 2> GetScreens() const;

        const Vector2& GetFrameBufferSize(Screen screen);

        // clang-format off
        static constexpr BidirectionalMap pixelFormats = {
            PIXELFORMAT_R8_UNORM,         GX2_SURFACE_FORMAT_UNORM_R8,
            PIXELFORMAT_R16_UNORM,        GX2_SURFACE_FORMAT_UNORM_R16,
            PIXELFORMAT_RG8_UNORM,        GX2_SURFACE_FORMAT_UNORM_R8_G8,
            PIXELFORMAT_RGBA8_UNORM,      GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8,
            PIXELFORMAT_RGB565_UNORM,     GX2_SURFACE_FORMAT_UNORM_R5_G6_B5,
            PIXELFORMAT_RGBA8_UNORM_SRGB, GX2_SURFACE_FORMAT_SRGB_R8_G8_B8_A8,
            PIXELFORMAT_DXT1_UNORM,       GX2_SURFACE_FORMAT_UNORM_BC1,
            PIXELFORMAT_DXT3_UNORM,       GX2_SURFACE_FORMAT_UNORM_BC2,
            PIXELFORMAT_DXT5_UNORM,       GX2_SURFACE_FORMAT_UNORM_BC3,
            PIXELFORMAT_BC4_UNORM,        GX2_SURFACE_FORMAT_UNORM_BC4,
            PIXELFORMAT_BC5_UNORM,        GX2_SURFACE_FORMAT_UNORM_BC5
        };

        static constexpr BidirectionalMap blendEquations = {
            RenderState::BLENDOP_ADD,              GX2_BLEND_COMBINE_MODE_ADD,
            RenderState::BLENDOP_SUBTRACT,         GX2_BLEND_COMBINE_MODE_SUB,
            RenderState::BLENDOP_REVERSE_SUBTRACT, GX2_BLEND_COMBINE_MODE_REV_SUB,
            RenderState::BLENDOP_MIN,              GX2_BLEND_COMBINE_MODE_MIN,
            RenderState::BLENDOP_MAX,              GX2_BLEND_COMBINE_MODE_MAX
        };

        static constexpr BidirectionalMap blendFactors = {
            RenderState::BLENDFACTOR_ZERO,                GX2_BLEND_MODE_ZERO,
            RenderState::BLENDFACTOR_ONE,                 GX2_BLEND_MODE_ONE,
            RenderState::BLENDFACTOR_SRC_COLOR,           GX2_BLEND_MODE_SRC_COLOR,
            RenderState::BLENDFACTOR_ONE_MINUS_SRC_COLOR, GX2_BLEND_MODE_INV_SRC_COLOR,
            RenderState::BLENDFACTOR_SRC_ALPHA,           GX2_BLEND_MODE_SRC_ALPHA,
            RenderState::BLENDFACTOR_ONE_MINUS_SRC_ALPHA, GX2_BLEND_MODE_INV_SRC_ALPHA,
            RenderState::BLENDFACTOR_DST_COLOR,           GX2_BLEND_MODE_DST_COLOR,
            RenderState::BLENDFACTOR_ONE_MINUS_DST_COLOR, GX2_BLEND_MODE_INV_DST_COLOR,
            RenderState::BLENDFACTOR_DST_ALPHA,           GX2_BLEND_MODE_DST_ALPHA,
            RenderState::BLENDFACTOR_ONE_MINUS_DST_ALPHA, GX2_BLEND_MODE_INV_DST_ALPHA,
            RenderState::BLENDFACTOR_SRC_ALPHA_SATURATED, GX2_BLEND_MODE_SRC_ALPHA_SAT
        };

        static constexpr BidirectionalMap filterModes = {
            SamplerState::FILTER_LINEAR,  GX2_TEX_XY_FILTER_MODE_LINEAR,
            SamplerState::FILTER_NEAREST, GX2_TEX_XY_FILTER_MODE_POINT
        };

        static constexpr BidirectionalMap wrapModes = {
            SamplerState::WRAP_CLAMP,           GX2_TEX_CLAMP_MODE_CLAMP,
            SamplerState::WRAP_CLAMP_ZERO,      GX2_TEX_CLAMP_MODE_CLAMP_BORDER,
            SamplerState::WRAP_REPEAT,          GX2_TEX_CLAMP_MODE_MIRROR,
            SamplerState::WRAP_MIRRORED_REPEAT, GX2_TEX_CLAMP_MODE_MIRROR
        };

        static constexpr BidirectionalMap cullModes = {
            vertex::CULL_NONE,  -1,
            vertex::CULL_BACK,  -2,
            vertex::CULL_FRONT, -3
        };

        static constexpr BidirectionalMap windingModes = {
            vertex::WINDING_CW,  GX2_FRONT_FACE_CW,
            vertex::WINDING_CCW, GX2_FRONT_FACE_CCW
        };

        static constexpr BidirectionalMap compareModes = {
            RenderState::COMPARE_LESS,     GX2_COMPARE_FUNC_LESS,
            RenderState::COMPARE_LEQUAL,   GX2_COMPARE_FUNC_LEQUAL,
            RenderState::COMPARE_EQUAL,    GX2_COMPARE_FUNC_EQUAL,
            RenderState::COMPARE_GEQUAL,   GX2_COMPARE_FUNC_GEQUAL,
            RenderState::COMPARE_GREATER,  GX2_COMPARE_FUNC_GREATER,
            RenderState::COMPARE_NOTEQUAL, GX2_COMPARE_FUNC_NOT_EQUAL,
            RenderState::COMPARE_ALWAYS,   GX2_COMPARE_FUNC_ALWAYS,
            RenderState::COMPARE_NEVER,    GX2_COMPARE_FUNC_NEVER
        };

        static constexpr BidirectionalMap primitiveModes = {
            vertex::PRIMITIVE_TRIANGLES,      GX2_PRIMITIVE_MODE_TRIANGLES,
            vertex::PRIMITIVE_TRIANGLE_FAN,   GX2_PRIMITIVE_MODE_TRIANGLE_FAN,
            vertex::PRIMITIVE_TRIANGLE_STRIP, GX2_PRIMITIVE_MODE_TRIANGLE_STRIP,
            vertex::PRIMITIVE_QUADS,          GX2_PRIMITIVE_MODE_QUADS,
            vertex::PRIMITIVE_POINTS,         (GX2PrimitiveMode)-1
        };

        static constexpr BidirectionalMap gfxScreens =  {
            "tv",      Screen::SCREEN_TV,
            "gamepad", Screen::SCREEN_GAMEPAD
        };
        // clang-format on

      private:
        // clang-format off
        static constexpr BidirectionalMap scanBuffers = {
            Screen::SCREEN_TV,      GX2_SCAN_TARGET_TV,
            Screen::SCREEN_GAMEPAD, GX2_SCAN_TARGET_DRC
        };
        // clang-format on

        struct Transform
        {
            glm::mat4 modelView;
            glm::mat4 projection;
        } transform;

        static uint32_t ProcUIAcquired(void* args);

        static uint32_t ProcUIReleased(void* args);

        int OnForegroundAcquired();

        int OnForegroundReleased();

        bool inForeground;
        void* commandBuffer;

        struct Framebuffer
        {
            uint8_t mode;
            void* scanBuffer;
            uint32_t scanBufferSize;
            GX2ColorBuffer buffer;
            Vector2 dimensions;
        };

        Framebuffer current;
        GX2ContextState* state;

        std::array<Framebuffer, MAX_RENDERTARGETS> framebuffers;
    };
} // namespace love
