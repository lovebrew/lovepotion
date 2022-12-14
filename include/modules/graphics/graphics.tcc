#pragma once

#include <common/color.hpp>
#include <common/console.hpp>
#include <common/math.hpp>
#include <common/module.hpp>
#include <common/strongreference.hpp>

#include <common/matrix_ext.hpp>

#include <objects/font/font.tcc>
#include <objects/texture/texture.tcc>

#include <objects/quad/quad.hpp>

#include <utilities/driver/renderer/renderer.tcc>
#include <utilities/driver/renderer/renderstate.hpp>
#include <utilities/driver/renderer/samplerstate.hpp>
#include <utilities/driver/renderer/vertex.hpp>

namespace love
{
    using OptionalColor  = std::optional<Color>;
    using OptionalInt    = std::optional<int>;
    using OptionalDouble = std::optional<double>;

    template<Console::Platform T = Console::ALL>
    class Graphics : public Module
    {
      public:
        static constexpr uint8_t MAX_USER_STACK_DEPTH = 0x80;

        enum DrawMode
        {
            DRAW_LINE,
            DRAW_FILL
        };

        enum ArcMode
        {
            ARC_OPEN,
            ARC_CLOSED,
            ARC_PIE
        };

        enum LineStyle
        {
            LINE_ROUGH,
            LINE_SMOOTH
        };

        enum LineJoin
        {
            LINE_JOIN_NONE,
            LINE_JOIN_MITER,
            LINE_JOIN_BEVEL
        };

        enum StackType
        {
            STACK_ALL,
            STACK_TRANSFORM
        };

        struct DisplayState
        {
            DisplayState()
            {
                this->defaultSamplerState.mipmapFilter = SamplerState::MIPMAP_FILTER_LINEAR;
            }

            Color foreground = { 1.0, 1.0, 1.0, 1.0 };
            Color background = { 0.0, 0.0, 0.0, 1.0 };

            RenderState::BlendState blendState = RenderState::ComputeBlendState(
                RenderState::BLEND_ALPHA, RenderState::BLENDALPHA_MULTIPLY);

            StrongReference<Font<Console::Which>> font;

            struct
            {
                float width     = 1.0f;
                LineStyle style = LINE_SMOOTH;
                LineJoin join   = LINE_JOIN_MITER;
            } line;

            float pointSize = 1.0f;

            struct
            {
                bool active = false;
                Rect bounds = {};
            } scissor;

            struct
            {
                RenderState::CompareMode mode = RenderState::COMPARE_ALWAYS;
                bool depthWrite               = false;
            } compare;

            Vertex::CullMode cullMode   = Vertex::CULL_NONE;
            Vertex::Winding windingMode = Vertex::WINDING_CCW;

            RenderState::ColorMask colorMask {};
            SamplerState defaultSamplerState = SamplerState {};
        };

        static bool gammaCorrectColor;

        static inline Screen activeScreen = (Screen)0;

        static void SetGammaCorrect(bool enable)
        {
            gammaCorrectColor = enable;
        }

        static bool IsGammaCorrect()
        {
            return gammaCorrectColor;
        }

        /* todo: needs math module */
        void GammaCorrectColor(Color& color)
        {}

        /* todo: needs math module */
        void UnGammaCorrectColor(Color& color);

        Graphics() :
            width(0),
            height(0),
            pixelWidth(0),
            pixelHeight(0),
            created(false),
            active(true)
        {
            this->transformStack.reserve(16);
            this->transformStack.push_back(Matrix4<Console::Which>());

            this->pixelScaleStack.reserve(16);
            this->pixelScaleStack.push_back(1.0);

            this->states.reserve(10);
            this->states.push_back(DisplayState());
        }

        virtual ~Graphics()
        {
            this->states.clear();
        }

        virtual ModuleType GetModuleType() const
        {
            return M_GRAPHICS;
        }

        virtual const char* GetName() const
        {
            return "love.graphics";
        }

        void SetFont(Font<Console::Which>* font)
        {
            this->states.back().font = font;
        }

        Font<Console::Which>* GetFont()
        {
            return this->states.back().font;
        }

        void Reset()
        {
            DisplayState state {};
            this->RestoreState(state);
            this->Origin();
        }

        bool IsCreated() const
        {
            return this->created;
        }

        bool IsActive() const
        {
            return this->active;
        }

        void SetActive(bool active)
        {
            this->active = active;
        }

        void PushTransform()
        {
            this->transformStack.push_back(this->transformStack.back());
        }

        void PushIdentityTransform()
        {
            this->transformStack.push_back(Matrix4<Console::Which>());
        }

        void PopTransform()
        {
            this->transformStack.pop_back();
        }

        const Matrix4<Console::Which>& GetTransform() const
        {
            return this->transformStack.back();
        }

        void Rotate(float r)
        {
            this->transformStack.back().Rotate(r);
        }

        void Scale(float x, float y)
        {
            this->transformStack.back().Scale(x, y);
            this->pixelScaleStack.back() *= (fabs(x) + fabs(y)) / 2.0;
        }

        void Translate(float x, float y)
        {
            this->transformStack.back().Translate(x, y);
        }

        void Shear(float kx, float ky)
        {
            this->transformStack.back().Shear(kx, ky);
        }

        void Origin()
        {
            this->transformStack.back().SetIdentity();
            this->pixelScaleStack.back() = 1;
        }

        void RestoreState(const DisplayState& state)
        {
            this->SetColor(state.foreground);
            this->SetBackgroundColor(state.background);

            /* todo: set blend state */

            this->SetLineWidth(state.line.width);
            this->SetLineStyle(state.line.style);
            this->SetLineJoin(state.line.join);

            this->SetPointSize(state.pointSize);

            if (state.scissor.active)
                this->SetScissor(state.scissor.bounds);
            else
                this->SetScissor();

            this->SetMeshCullMode(state.cullMode);
            this->SetFrontFaceWinding(state.windingMode);

            this->SetColorMask(state.colorMask);
            this->SetDefaultSamplerState(state.defaultSamplerState);
        }

        // void RestoreStateChecked(const DisplayStateBase& state);

        void SetActiveScreen(Screen screen)
        {
            this->activeScreen = screen;
        }

        const Screen GetActiveScreen() const
        {
            return this->activeScreen;
        }

        void SetColor(const Color& color)
        {
            this->states.back().foreground = color;
        }

        const Color GetColor() const
        {
            return this->states.back().foreground;
        }

        void SetBackgroundColor(const Color& color)
        {
            this->states.back().background = color;
        }

        const Color GetBackgroundColor() const
        {
            return this->states.back().background;
        }

        void SetLineWidth(float width)
        {
            this->states.back().line.width = width;
        }

        const float GetLineWidth() const
        {
            return this->states.back().line.width;
        }

        void SetLineStyle(LineStyle style)
        {
            this->states.back().line.style = style;
        }

        const LineStyle GetLineStyle() const
        {
            return this->states.back().line.style;
        }

        void SetLineJoin(LineJoin join)
        {
            this->states.back().line.join = join;
        }

        const LineJoin GetLineJoin() const
        {
            return this->states.back().line.join;
        }

        void SetPointSize(float size)
        {
            this->states.back().pointSize = size;
        }

        const float GetPointSize() const
        {
            return this->states.back().pointSize;
        }

        void SetScissor(const Rect& scissor)
        {
            this->states.back().scissor.bounds = scissor;
            this->states.back().scissor.active = true;
        }

        void SetScissor()
        {
            this->states.back().scissor.active = false;
        }

        void SetMeshCullMode(Vertex::CullMode mode)
        {
            this->states.back().cullMode = mode;
        }

        const Vertex::CullMode GetMeshCullMode() const
        {
            return this->states.back().cullMode;
        }

        void SetFrontFaceWinding(Vertex::Winding winding)
        {
            this->states.back().windingMode = winding;
        }

        const Vertex::Winding GetFrontFaceWinding() const
        {
            return this->states.back().windingMode;
        }

        void SetColorMask(const RenderState::ColorMask& mask)
        {
            this->states.back().colorMask = mask;
        }

        const RenderState::ColorMask GetColorMask() const
        {
            return this->states.back().colorMask;
        }

        void SetDefaultSamplerState(const SamplerState& state)
        {
            this->states.back().defaultSamplerState = state;
        }

        const SamplerState& GetDefaultSamplerState() const
        {
            return this->states.back().defaultSamplerState;
        }

        /* todo: this should be called in the child class via SetBlendState */
        void SetBlendMode(RenderState::BlendMode mode, RenderState::BlendAlpha alphaMode)
        {
            if (alphaMode == RenderState::BLENDALPHA_MULTIPLY &&
                !RenderState::IsAlphaMultiplyBlendSupported(mode))
            {
                std::optional<const char*> modeOpt;
                if (!(modeOpt = RenderState::blendModes.ReverseFind(mode)))
                    modeOpt = "unknown";

                throw love::Exception("The '%s' blend mode must be used with premultiplied alpha.",
                                      *modeOpt);
            }
        }

        const RenderState::BlendState& GetBlendState() const
        {
            return this->states.back().blendState;
        }

        const RenderState::BlendMode GetBlendMode(RenderState::BlendAlpha& alphaMode)
        {
            return RenderState::ComputeBlendMode(this->states.back().blendState, alphaMode);
        }

        PixelFormat GetSizedFormat(PixelFormat format, bool rendertarget, bool readable) const
        {
            uint32_t requiredFlags = 0;

            if (rendertarget)
                requiredFlags |= PIXELFORMAT_USAGE_FLAGS_RENDERTARGET;

            if (readable)
                requiredFlags |= PIXELFORMAT_USAGE_FLAGS_SAMPLE;

            switch (format)
            {
                case PIXELFORMAT_NORMAL:
                    return PIXELFORMAT_RGBA8_UNORM;
                case PIXELFORMAT_HDR:
                    return PIXELFORMAT_RGBA16_FLOAT;
                default:
                    return format;
            }
        }

        Quad* NewQuad(const Quad::Viewport& viewport, double sourceWidth, double sourceHeight) const
        {
            return new Quad(viewport, sourceWidth, sourceHeight);
        }

      protected:
        std::vector<double> pixelScaleStack;
        std::vector<Matrix4<Console::Which>> transformStack;

        std::vector<DisplayState> states;
        std::vector<StackType> stackTypeStack;

        int width;
        int height;

        int pixelWidth;
        int pixelHeight;

        bool created;
        bool active;

        StrongReference<Font<Console::Which>> defaultFont;
    };
} // namespace love
