#pragma once

#include <utilities/driver/renderer/renderer.tcc>

#include <common/color.hpp>
#include <common/math.hpp>
#include <common/pixelformat.hpp>

#include <utilities/driver/framebuffer_ext.hpp>
#include <utilities/driver/renderer/drawcommand.tcc>
#include <utilities/driver/renderer/renderstate.hpp>
#include <utilities/driver/renderer/samplerstate.hpp>
#include <utilities/driver/renderer/vertex.hpp>

#include <vector>

#include <3ds.h>
#include <citro2d.h>

namespace love
{
    template<>
    class Renderer<Console::CTR> : public Renderer<Console::ALL>
    {
      private:
        static constexpr const char* RENDERER_NAME    = "citro3d";
        static constexpr const char* RENDERER_VERSION = "1.7.0";
        static constexpr const char* RENDERER_VENDOR  = "devkitPro";
        static constexpr const char* RENDERER_DEVICE  = "DMP PICA200";

        static inline constexpr int MAX_OBJECTS        = 0x1000;
        static inline constexpr int VERTEX_BUFFER_SIZE = 6 * MAX_OBJECTS;
        static inline constexpr auto TOTAL_BUFFER_SIZE = VERTEX_BUFFER_SIZE * VERTEX_SIZE;

        static inline constexpr uint8_t MAX_RENDERTARGETS = 0x03;

        Renderer();

      public:
        static Renderer& Instance()
        {
            static Renderer instance;
            return instance;
        }

        ~Renderer();

        static void FlushVertices();

        Info GetRendererInfo();

        void DestroyFramebuffers();

        void CreateFramebuffers();

        void Clear(const Color& color);

        void ClearDepthStencil(int stencil, uint8_t mask, double depth);

        void SetBlendColor(const Color& color);

        void SetBlendMode(const RenderState::BlendState& state);

        void EnsureInFrame();

        /* todo: canvases */
        void BindFramebuffer(Texture<Console::ALL>* texture = nullptr);

        bool Render(DrawCommand& command);

        void Present();

        void SetViewport(const Rect& viewport, bool tilt);

        void SetScissor(const Rect& scissor, bool canvasActive);

        void SetStencil(RenderState::CompareMode mode, int value);

        void SetMeshCullMode(vertex::CullMode mode);

        void SetVertexWinding(vertex::Winding winding);

        void SetSamplerState(Texture<Console::CTR>* texture, SamplerState& state);

        void SetColorMask(const RenderState::ColorMask& mask);

        Framebuffer<Console::CTR>& GetCurrent()
        {
            return this->targets[love::GetActiveScreen()];
        }

        C3D_Mtx& GetModelView()
        {
            return this->context.modelView;
        }

        void SetWideMode(bool enable)
        {
            this->ModeChanged([enable]() { gfxSetWide(enable); });
        }

        const bool GetWide() const
        {
            return gfxIsWide();
        }

        void Set3D(bool enable)
        {
            this->ModeChanged([enable]() { gfxSet3D(enable); });
        }

        const bool Get3D() const
        {
            return gfxIs3D();
        }

        /* used for Canvases */
        void DeferCallToEndOfFrame(std::function<void()>&& function)
        {
            this->deferred.emplace_back(std::move(function));
        }

        // clang-format off
        static constexpr BidirectionalMap primitiveModes =
        {
            vertex::PRIMITIVE_TRIANGLES,      GPU_TRIANGLES,
            vertex::PRIMITIVE_TRIANGLE_STRIP, GPU_TRIANGLE_STRIP,
            vertex::PRIMITIVE_TRIANGLE_FAN,   GPU_TRIANGLE_FAN
        };
        // clang-format on

        // clang-format off
        static constexpr BidirectionalMap pixelFormats = {
            PIXELFORMAT_RGBA8_UNORM,  GPU_RGBA8,
            PIXELFORMAT_RGB8,         GPU_RGB8,
            PIXELFORMAT_RGB565_UNORM, GPU_RGB565,
            PIXELFORMAT_LA8_UNORM,    GPU_LA8
        };

        static constexpr BidirectionalMap blendEquations = {
            RenderState::BLENDOP_ADD,                GPU_BLEND_ADD,
            RenderState::BLENDOP_SUBTRACT,           GPU_BLEND_SUBTRACT,
            RenderState::BLENDOP_REVERSE_SUBTRACT,   GPU_BLEND_REVERSE_SUBTRACT,
            RenderState::BLENDOP_MIN,                GPU_BLEND_MIN,
            RenderState::BLENDOP_MAX,                GPU_BLEND_MAX
        };

        static constexpr BidirectionalMap blendFactors = {
            RenderState::BLENDFACTOR_ZERO,                GPU_ZERO,
            RenderState::BLENDFACTOR_ONE,                 GPU_ONE,
            RenderState::BLENDFACTOR_SRC_COLOR,           GPU_SRC_COLOR,
            RenderState::BLENDFACTOR_ONE_MINUS_SRC_COLOR, GPU_ONE_MINUS_SRC_COLOR,
            RenderState::BLENDFACTOR_SRC_ALPHA,           GPU_SRC_ALPHA,
            RenderState::BLENDFACTOR_ONE_MINUS_SRC_ALPHA, GPU_ONE_MINUS_SRC_ALPHA,
            RenderState::BLENDFACTOR_DST_COLOR,           GPU_DST_COLOR,
            RenderState::BLENDFACTOR_ONE_MINUS_DST_COLOR, GPU_ONE_MINUS_DST_COLOR,
            RenderState::BLENDFACTOR_DST_ALPHA,           GPU_DST_ALPHA,
            RenderState::BLENDFACTOR_ONE_MINUS_DST_ALPHA, GPU_ONE_MINUS_DST_ALPHA,
            RenderState::BLENDFACTOR_SRC_ALPHA_SATURATED, GPU_SRC_ALPHA_SATURATE
        };

        static constexpr BidirectionalMap filterModes = {
            SamplerState::FILTER_LINEAR,  GPU_LINEAR,
            SamplerState::FILTER_NEAREST, GPU_NEAREST
        };

        static constexpr BidirectionalMap wrapModes = {
            SamplerState::WRAP_CLAMP,           GPU_CLAMP_TO_EDGE,
            SamplerState::WRAP_CLAMP_ZERO,      GPU_CLAMP_TO_BORDER,
            SamplerState::WRAP_REPEAT,          GPU_REPEAT,
            SamplerState::WRAP_MIRRORED_REPEAT, GPU_MIRRORED_REPEAT
        };

        static constexpr BidirectionalMap cullModes = {
            vertex::CULL_NONE,  GPU_CULL_NONE,
            vertex::CULL_BACK,  GPU_CULL_BACK_CCW,
            vertex::CULL_FRONT, GPU_CULL_FRONT_CCW
        };

        static constexpr BidirectionalMap compareModes = {
            RenderState::COMPARE_LESS,     GPU_LESS,
            RenderState::COMPARE_LEQUAL,   GPU_LEQUAL,
            RenderState::COMPARE_EQUAL,    GPU_EQUAL,
            RenderState::COMPARE_GEQUAL,   GPU_GEQUAL,
            RenderState::COMPARE_GREATER,  GPU_GREATER,
            RenderState::COMPARE_NOTEQUAL, GPU_NOTEQUAL,
            RenderState::COMPARE_ALWAYS,   GPU_ALWAYS,
            RenderState::COMPARE_NEVER,    GPU_NEVER
        };
        // clang-format on

      private:
        template<typename T>
        void ModeChanged(const T& func)
        {
            this->DestroyFramebuffers();
            func();
            this->CreateFramebuffers();
        }

        struct Context
        {
            C3D_Mtx modelView;
            C3D_Mtx projection;
            C3D_RenderTarget* target;
            bool dirtyProjection;

            vertex::CullMode cullMode;

            love::RenderState::ColorMask colorMask;
            love::RenderState::BlendState blendState;
            love::RenderState::StencilState stencilState;
        } context;

        std::vector<std::function<void()>> deferred;
        std::array<Framebuffer<Console::CTR>, MAX_RENDERTARGETS> targets;

        C3D_Tex* currentTexture;

        static inline PrimitiveType currentPrimitiveType = PRIMITIVE_MAX_ENUM;
        static inline int totalVertices                  = 0;

        static inline std::vector<DrawCommand> m_commands {};

        C3D_BufInfo bufferInfo;

        static inline CommonFormat m_format = CommonFormat::NONE;
        static inline Vertex* m_vertices    = nullptr;
    };
} // namespace love
