#pragma once

#include <utilities/driver/renderer/renderer.tcc>

#include <common/color.hpp>
#include <common/math.hpp>
#include <common/pixelformat.hpp>

#include <utilities/driver/renderer/renderstate.hpp>
#include <utilities/driver/renderer/samplerstate.hpp>
#include <utilities/driver/renderer/vertex.hpp>

#include <utilities/bidirectionalmap/smallvector.hpp>

#include <common/luax.hpp>

#include <array>

#include <3ds.h>
#include <citro2d.h>

namespace love
{
    enum class Screen : uint8_t
    {
        SCREEN_LEFT,
        SCREEN_RIGHT,
        SCREEN_BOTTOM
    };

    template<>
    class Renderer<Console::CTR> : public Renderer<Console::ALL>
    {
      private:
        static constexpr const char* RENDERER_NAME    = "citro3d";
        static constexpr const char* RENDERER_VERSION = "1.7.0";
        static constexpr const char* RENDERER_VENDOR  = "devkitPro";
        static constexpr const char* RENDERER_DEVICE  = "DMP PICA200";

        static constexpr uint8_t MAX_RENDERTARGETS = 0x03;

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
        void BindFramebuffer(/* Canvas* canvas = nullptr*/);

        void Present();

        void SetViewport(const Rect& viewport);

        void SetScissor(bool enable, const Rect& scissor, bool canvasActive);

        void SetStencil(RenderState::CompareMode mode, int value);

        void SetMeshCullMode(Vertex::CullMode mode);

        void SetVertexWinding(Vertex::Winding winding);

        void SetSamplerState(Texture* texture, SamplerState& state);

        void SetColorMask(const RenderState::ColorMask& mask);

        void SetLineWidth(float lineWidth);

        /* todo -- need Graphics */
        void SetLineStyle();

        void SetPointSize(float size);

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

        int CheckScreen(lua_State* L, const char* name, Screen* screen) const;

        std::vector<std::string_view> GetScreens() const
        {
            std::vector<std::string_view> names;

            if (this->Get3D())
            {
                for (const auto& name : gfx3dScreens.GetNames())
                    names.push_back(name);
            }
            else
            {
                for (const auto& name : gfx2dScreens.GetNames())
                    names.push_back(name);
            }

            return names;
        }

        // clang-format off
        static constexpr BidirectionalMap pixelFormats = {
            PIXELFORMAT_RGBA8_UNORM,  GPU_RGBA8,
            PIXELFORMAT_RGB8,         GPU_RGB8,
            PIXELFORMAT_RGB565_UNORM, GPU_RGB565,
            PIXELFORMAT_LA8_UNORM,    GPU_LA8,
            PIXELFORMAT_ETC1_UNORM,   GPU_ETC1
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
            Vertex::CULL_NONE,  GPU_CULL_NONE,
            Vertex::CULL_BACK,  GPU_CULL_BACK_CCW,
            Vertex::CULL_FRONT, GPU_CULL_FRONT_CCW
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

        static constexpr BidirectionalMap gfx3dScreens = {
            "left",   Screen::SCREEN_LEFT,
            "right",  Screen::SCREEN_RIGHT,
            "bottom", Screen::SCREEN_BOTTOM
        };

        static constexpr BidirectionalMap gfx2dScreens = {
            "top",    Screen::SCREEN_LEFT,
            "bottom", Screen::SCREEN_BOTTOM
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

        std::vector<std::function<void()>> deferred;
        std::array<C3D_RenderTarget*, MAX_RENDERTARGETS> targets;

        C3D_RenderTarget* current;
    };
} // namespace love
