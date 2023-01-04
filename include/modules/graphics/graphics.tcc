#pragma once

#include <common/color.hpp>
#include <common/console.hpp>
#include <common/math.hpp>
#include <common/module.hpp>
#include <common/strongreference.hpp>

#include <common/matrix_ext.hpp>

#include <modules/math/math.hpp>

#include <objects/font/font.tcc>
#include <objects/shader/shader.tcc>
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

        struct Stats
        {
            int drawCalls;
            int drawCallsBatched;
            int renderTargetSwitches;
            int shaderSwitches;
            int textures;
            int fonts;
            int64_t textureMemory;
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
            StrongReference<Shader<Console::Which>> shader;
            StrongReference<Texture<Console::Which>> renderTarget;

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

            vertex::CullMode cullMode   = vertex::CULL_NONE;
            vertex::Winding windingMode = vertex::WINDING_CCW;

            RenderState::ColorMask colorMask {};
            SamplerState defaultSamplerState = SamplerState {};
        };

        static inline bool gammaCorrectColor = false;

        static void SetGammaCorrect(bool enable)
        {
            gammaCorrectColor = enable;
        }

        static bool IsGammaCorrect()
        {
            return gammaCorrectColor;
        }

        static void GammaCorrectColor(Color& color)
        {
            if (Graphics::IsGammaCorrect())
            {
                color.r = Math::GammaToLinear(color.r);
                color.g = Math::GammaToLinear(color.g);
                color.a = Math::GammaToLinear(color.b);
            }
        }

        static Color GammaCorrectColor(const Color& color)
        {
            Color result = color;
            Graphics::GammaCorrectColor(result);

            return result;
        }

        static void UnGammaCorrectColor(Color& color)
        {
            if (Graphics::IsGammaCorrect())
            {
                color.r = Math::LinearToGamma(color.r);
                color.g = Math::LinearToGamma(color.g);
                color.a = Math::LinearToGamma(color.b);
            }
        }

        static Color UnGammaCorrectColor(const Color& color)
        {
            Color result = color;
            Graphics::UnGammaCorrectColor(result);

            return result;
        }

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
            this->defaultFont.Set(nullptr);
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

        void SetShader()
        {
            this->states.back().shader.Set(nullptr);
        }

        void SetShader(Shader<Console::Which>* shader)
        {
            if (shader == nullptr)
                return this->SetShader();
        }

        Shader<Console::Which>* GetShader() const
        {
            return this->states.back().shader.Get();
        }

        Stats GetStats() const
        {
            Stats stats {};

            stats.drawCalls     = Renderer<>::drawCalls;
            stats.textures      = Texture<>::textureCount;
            stats.fonts         = Font<>::fontCount;
            stats.textureMemory = Texture<>::totalGraphicsMemory;

            return stats;
        }

        void Reset()
        {
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

        void Push(StackType type)
        {
            if (this->stackTypeStack.size() == MAX_USER_STACK_DEPTH)
                throw love::Exception("Maximum stack depth reached (more pushes than pops?)");

            this->PushTransform();
            this->pixelScaleStack.push_back(this->pixelScaleStack.back());

            if (type == STACK_ALL)
                this->states.push_back(this->states.back());

            this->stackTypeStack.push_back(type);
        }

        void Pop()
        {
            if (this->stackTypeStack.size() < 1)
                throw love::Exception("Minimum stack depth reached (more pops than pushes?)");

            this->PopTransform();
            this->pixelScaleStack.pop_back();
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

        void ApplyTransform(const Matrix4<Console::Which>& matrix)
        {
            auto& current = this->transformStack.back();
            current *= matrix;

            float scaleX, scaleY;
            current.GetApproximateScale(scaleX, scaleY);
            this->pixelScaleStack.back() = (scaleX + scaleY) / 2.0;
        }

        void ReplaceTransform(const Matrix4<Console::Which>& matrix)
        {
            this->transformStack.back() = matrix;

            float scaleX, scaleY;
            matrix.GetApproximateScale(scaleX, scaleY);
            this->pixelScaleStack.back() = (scaleX + scaleY) / 2.0;
        }

        Vector2 TransformPoint(Vector2 point)
        {
            Vector2 result {};
            this->transformStack.back().TransformXY(&result, &point, 1);

            return result;
        }

        Vector2 InverseTransformPoint(Vector2 point)
        {
            Vector2 result {};
            this->transformStack.back().Inverse().TransformXY(&result, &point, 1);

            return result;
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
            auto& transform = this->transformStack.back();
            transform.SetIdentity();

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

            this->SetMeshCullMode(state.cullMode);
            this->SetFrontFaceWinding(state.windingMode);

            this->SetFont(state.font.Get());
            this->SetShader(state.shader.Get());

            this->SetColorMask(state.colorMask);
            this->SetDefaultSamplerState(state.defaultSamplerState);
        }

        bool IsPixelFormatSupported(PixelFormat format, uint32_t usage, bool isSRGB = false)
        {
            if (isSRGB)
                format = love::GetSRGBPixelFormat(format);

            bool rendertarget = (usage & PIXELFORMAT_USAGE_FLAGS_RENDERTARGET) != 0;
            bool readable     = (usage & PIXELFORMAT_USAGE_FLAGS_SAMPLE) != 0;

            format = this->GetSizedFormat(format, rendertarget, readable);

            /* todo: calculate pixel format usages*/
            return false;
        }

        bool IsRenderTargetActive() const
        {
            const auto& renderTarget = states.back().renderTarget;
            return renderTarget != nullptr;
        }

        bool IsRenderTargetActive(Texture<Console::Which>* texture) const
        {
            const auto& renderTarget = states.back().renderTarget;

            if (renderTarget.Get() == texture)
                return true;

            return false;
        }

        void RestoreStateChecked(const DisplayState& state)
        {
            const DisplayState& current = this->states.back();

            if (state.foreground != current.foreground)
                this->SetColor(state.foreground);

            this->SetBackgroundColor(state.background);

            /* todo set blend state */

            this->SetLineWidth(state.line.width);
            this->SetLineStyle(state.line.style);
            this->SetLineJoin(state.line.join);

            if (state.pointSize != current.pointSize)
                this->SetPointSize(state.pointSize);

            this->SetMeshCullMode(state.cullMode);

            if (state.windingMode != current.windingMode)
                this->SetFrontFaceWinding(state.windingMode);

            this->SetFont(state.font.Get());
            this->SetShader(state.shader.Get());

            if (state.colorMask != current.colorMask)
                this->SetColorMask(state.colorMask);

            this->SetDefaultSamplerState(state.defaultSamplerState);
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

        void SetMeshCullMode(vertex::CullMode mode)
        {
            this->states.back().cullMode = mode;
        }

        const vertex::CullMode GetMeshCullMode() const
        {
            return this->states.back().cullMode;
        }

        void SetFrontFaceWinding(vertex::Winding winding)
        {
            this->states.back().windingMode = winding;
        }

        const vertex::Winding GetFrontFaceWinding() const
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

        // clang-format off
        static constexpr BidirectionalMap fillModes = {
            "fill", DRAW_FILL,
            "line", DRAW_LINE
        };

        static constexpr BidirectionalMap stackTypes = {
            "all",       STACK_ALL,
            "transform", STACK_TRANSFORM
        };

        static constexpr BidirectionalMap arcModes = {
            "open",   ARC_OPEN,
            "closed", ARC_CLOSED,
            "pie",    ARC_PIE
        };

        static constexpr BidirectionalMap lineStyles = {
            "rough",  LINE_ROUGH,
            "smooth", LINE_SMOOTH,
        };

        static constexpr BidirectionalMap lineJoins = {
            "none",  LINE_JOIN_NONE,
            "miter", LINE_JOIN_MITER,
            "bevel", LINE_JOIN_BEVEL
        };
        // clang-format on

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
