#pragma once

#include <common/color.hpp>
#include <common/console.hpp>
#include <common/math.hpp>
#include <common/module.hpp>
#include <common/optional.hpp>
#include <common/strongreference.hpp>

#include <common/matrix.hpp>

#include <modules/font/fontmodule.tcc>
#include <modules/math/math.hpp>
#include <modules/window/window.tcc>

#include <objects/shader/shader.tcc>
#include <objects/texture/texture.tcc>

#include <objects/font/font.hpp>
#include <objects/mesh/mesh.hpp>
#include <objects/quad/quad.hpp>
#include <objects/spritebatch/spritebatch.hpp>
#include <objects/textbatch/textbatch.hpp>

#include <utilities/driver/renderer/drawcommand.tcc>

#include <utilities/driver/renderer/polyline/polyline.hpp>

#include <utilities/driver/renderer/polyline/types/beveljoin.hpp>
#include <utilities/driver/renderer/polyline/types/miterjoin.hpp>
#include <utilities/driver/renderer/polyline/types/nonejoin.hpp>

#include <utilities/driver/renderer/renderstate.hpp>
#include <utilities/driver/renderer/samplerstate.hpp>
#include <utilities/driver/renderer/vertex.hpp>
#include <utilities/driver/renderer_ext.hpp>

#include <cmath>

namespace love
{
    using OptionalColor = Optional<Color>;

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

        enum StackType
        {
            STACK_ALL,
            STACK_TRANSFORM
        };

        enum TemporaryRenderTargetFlags
        {
            TEMPORARY_RT_DEPTH   = (1 << 0),
            TEMPORARY_RT_STENCIL = (1 << 1),
        };

        struct RenderTargetStrongReference;

        struct RenderTarget
        {
          public:
            RenderTarget(Texture<Console::ALL>* texture, int slice = 0, int mipmap = 0) :
                texture(texture),
                slice(slice),
                mipmap(mipmap)
            {}

            RenderTarget() : texture(nullptr), slice(0), mipmap(0)
            {}

            bool operator!=(const RenderTarget& other) const
            {
                return this->texture != other.texture || this->slice != other.slice ||
                       mipmap != other.mipmap;
            }

            bool operator!=(const RenderTargetStrongReference& other) const
            {
                return this->texture != other.texture.Get() || this->slice != other.slice ||
                       this->mipmap != other.mipmap;
            }

            Texture<Console::ALL>* texture;
            int slice;
            int mipmap;
        };

        struct RenderTargetStrongReference
        {
          public:
            RenderTargetStrongReference(Texture<Console::ALL>* texture, int slice = 0,
                                        int mipmap = 0) :
                texture(texture),
                slice(slice),
                mipmap(mipmap)
            {}

            bool operator!=(const RenderTargetStrongReference& other) const
            {
                return this->texture.Get() != other.texture.Get() || this->slice != other.slice ||
                       this->mipmap != other.mipmap;
            }

            bool operator!=(const RenderTarget& other) const
            {
                return this->texture.Get() != other.texture || this->slice != other.slice ||
                       this->mipmap != other.mipmap;
            }

            StrongReference<Texture<Console::ALL>> texture;
            int slice  = 0;
            int mipmap = 0;
        };

        struct RenderTargets
        {
          public:
            std::vector<RenderTarget> colors;
            RenderTarget depthStencil;
            uint32_t temporaryFlags;

            RenderTargets() : depthStencil(nullptr), temporaryFlags(0)
            {}

            const RenderTarget& GetFirstTarget() const
            {
                return colors.empty() ? depthStencil : this->colors[0];
            }

            bool operator==(const RenderTargets& other) const
            {
                size_t totalColors = colors.size();
                if (totalColors != other.colors.size())
                    return false;

                for (size_t index = 0; index < totalColors; index++)
                {
                    if (this->colors[index] != other.colors[index])
                        return false;
                }

                if (this->depthStencil != other.depthStencil ||
                    this->temporaryRTFlags != other.temporaryRTFlags)
                {
                    return false;
                }

                return true;
            }
        };

        struct RenderTargetsStrongReference
        {
          public:
            RenderTargetsStrongReference() : depthStencil(nullptr), temporaryFlags(0)
            {}

            const RenderTargetStrongReference& GetFirstTarget() const
            {
                return this->colors.empty() ? this->depthStencil : this->colors[0];
            }

            std::vector<RenderTargetStrongReference> colors;
            RenderTargetStrongReference depthStencil;
            uint_fast32_t temporaryFlags;
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

            float gpuTime;
            float cpuTime;
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

            StrongReference<Font> font;
            StrongReference<Shader<Console::Which>> shader;
            RenderTargetsStrongReference renderTargets;

            struct
            {
                float width                  = 1.0f;
                RenderState::LineStyle style = RenderState::LINE_SMOOTH;
                RenderState::LineJoin join   = RenderState::LINE_JOIN_MITER;
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
            active(true),
            renderTargetSwitchCount(0)
        {
            this->transformStack.reserve(16);
            this->transformStack.push_back(Matrix4());

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

        void Clear(OptionalColor color, OptionalInt stencil, OptionalDouble depth)
        {
            Renderer<Console::Which>::Instance().BindFramebuffer();

            if (color.hasValue)
            {
                // GammaCorrectColor(color);
                Renderer<Console::Which>::Instance().Clear(color.value);
            }

            if (stencil.hasValue && depth.hasValue)
                Renderer<Console::Which>::Instance().ClearDepthStencil(stencil.value, 0xFF,
                                                                       depth.value);
        }

        void Clear(std::vector<OptionalColor>& colors, OptionalInt stencil, OptionalDouble depth)
        {
            int colorCount = colors.size();

            if (colorCount == 0 || !stencil.hasValue || !depth.hasValue)
                this->Clear(colorCount > 0 ? colors[0] : Color {}, stencil, depth);
        }

        void Present()
        {
            Renderer<Console::Which>::Instance().Present();

            Renderer<Console::Which>::drawCalls        = 0;
            Renderer<Console::Which>::drawCallsBatched = 0;
            Shader<Console::Which>::shaderSwitches     = 0;
        }

        /* graphics state */

        void Reset()
        {
            DisplayState state {};
            this->RestoreState(state);
            this->Origin();
        }

        void Pop()
        {
            if (this->stackTypeStack.size() < 1)
                throw love::Exception("Minimum stack depth reached (more pops than pushes?)");

            this->PopTransform();
            this->pixelScaleStack.pop_back();

            if (this->stackTypeStack.back() == STACK_ALL)
            {
                DisplayState& newState = this->states[this->states.size() - 2];
                this->RestoreStateChecked(newState);
                this->states.pop_back();
            }

            this->stackTypeStack.pop_back();
        }

        /* objects */

        Font* NewFont(Rasterizer* data) const
        {
            return new Font(data, this->states.back().defaultSamplerState);
        }

        void SetFont(Font* font)
        {
            this->states.back().font.Set(font);
        }

        Font* GetFont()
        {
            this->CheckSetDefaultFont();
            return this->states.back().font.Get();
        }

        SpriteBatch* NewSpriteBatch(Texture<Console::Which>* texture, int size) const
        {
            return new SpriteBatch(texture, size);
        }

        TextBatch* NewTextBatch(Font* font, const ColoredStrings& text = {}) const
        {
            return new TextBatch(font, text);
        }

        Mesh* NewMesh(int vertexCount, vertex::PrimitiveType mode) const
        {
            return new Mesh(vertexCount, mode);
        }

        Mesh* NewMesh(const void* data, size_t size, vertex::PrimitiveType mode) const
        {
            return new Mesh(data, size, mode);
        }

        void SetRenderTargetsInternal(const RenderTargets& targets, int width, int height,
                                      bool hasSRGBTexture)
        {
            // const DisplayState& state = this->states.back();

            bool isWindow = targets.GetFirstTarget().texture == nullptr;

            if (isWindow)
                Renderer<Console::Which>::Instance().BindFramebuffer();
            else
            {
                Renderer<Console::Which>::Instance().BindFramebuffer(
                    targets.GetFirstTarget().texture);
            }
        }

        void SetRenderTarget()
        {
            DisplayState& state = this->states.back();

            if (state.renderTargets.colors.empty() &&
                state.renderTargets.depthStencil.texture == nullptr)
            {
                return;
            }

            Renderer<Console::Which>::FlushVertices();
            this->SetRenderTargetsInternal(RenderTargets {}, this->width, this->height,
                                           this->IsGammaCorrect());

            state.renderTargets = RenderTargetsStrongReference();
            renderTargetSwitchCount++;
        }

        void SetRenderTarget(RenderTarget target, uint32_t flags)
        {
            if (target.texture == nullptr)
                return SetRenderTarget();

            RenderTargets targets {};
            targets.colors.push_back(target);
            targets.temporaryFlags = flags;

            this->SetRenderTargets(targets);
        }

        void SetRenderTargets(const RenderTargetsStrongReference& strongTargets)
        {
            RenderTargets targets {};
            targets.colors.reserve(strongTargets.colors.size());

            for (const auto& target : strongTargets.colors)
                targets.colors.emplace_back(target.texture.Get(), target.slice, target.mipmap);

            targets.depthStencil =
                RenderTarget(strongTargets.depthStencil.texture, strongTargets.depthStencil.slice,
                             strongTargets.depthStencil.mipmap);

            targets.temporaryFlags = strongTargets.temporaryFlags;
            return this->SetRenderTargets(targets);
        }

        void SetRenderTargets(const RenderTargets& targets)
        {
            DisplayState& state = this->states.back();
            int count           = (int)targets.colors.size();

            RenderTarget firstTarget            = targets.GetFirstTarget();
            Texture<Console::ALL>* firstTexture = firstTarget.texture;

            if (firstTexture == nullptr)
                return this->SetRenderTarget();

            const auto& previousTargets = state.renderTargets;

            /* validate if the rendertargets changed */
            if (count == (int)previousTargets.colors.size())
            {
                bool modified = false;

                for (int index = 0; index < count; index++)
                {
                    if (targets.colors[index] != previousTargets.colors[index])
                    {
                        modified = true;
                        break;
                    }
                }

                if (!modified && targets.depthStencil != previousTargets.depthStencil)
                    modified = true;

                if (targets.temporaryFlags != previousTargets.temporaryFlags)
                    modified = true;

                if (!modified)
                    return;
            }

            if (count > 0x01)
            {
                throw love::Exception("This system can't simultaneously render to %d textures",
                                      count);
            }

            PixelFormat firstColorFormat = PIXELFORMAT_UNKNOWN;
            if (!targets.colors.empty())
                firstColorFormat = targets.colors[0].texture->GetPixelFormat();

            if (!firstTexture->IsRenderTarget())
            {
                throw love::Exception(
                    "Texture must be created as a render target to be used in setRenderTargets.");
            }

            if (love::IsPixelFormatDepthStencil(firstColorFormat))
            {
                throw love::Exception(
                    "Depth/stencil format textures must be used with the 'depthstencil' field of "
                    "the table passed into setRenderTargets.");
            }

            if (firstTarget.mipmap < 0 || firstTarget.mipmap >= firstTexture->GetMipmapCount())
                throw love::Exception("Invalid mipmap level %d.", firstTarget.mipmap + 1);

            if (!firstTexture->IsValidSlice(firstTarget.slice, firstTarget.mipmap))
                throw love::Exception("Invalid slice index: %d.", firstTarget.slice + 1);

            bool hasSRGBtexture = love::IsPixelFormatSRGB(firstColorFormat);
            int pixelWidth      = firstTexture->GetPixelWidth(firstTarget.mipmap);
            int pixelHeight     = firstTexture->GetPixelHeight(firstTarget.mipmap);
            int reqMSAA         = firstTexture->GetRequestedMSAA();

            bool multiformatsupported = false;

            /* do a ton of validation - NOTE: only ONE rendertarget allowed MAX */
            for (int i = 1; i < count; i++)
            {
                Texture<Console::ALL>* canvas = targets.colors[i].texture;
                PixelFormat format            = canvas->GetPixelFormat();
                int mip                       = targets.colors[i].mipmap;
                int slice                     = targets.colors[i].slice;

                if (!canvas->IsRenderTarget())
                    throw love::Exception("Texture must be created as a render target to be used "
                                          "in setRenderTargets.");

                if (mip < 0 || mip >= canvas->GetMipmapCount())
                    throw love::Exception("Invalid mipmap level %d.", mip + 1);

                if (!canvas->IsValidSlice(slice, mip))
                    throw love::Exception("Invalid slice index: %d.", slice + 1);

                if (canvas->GetPixelWidth(mip) != pixelWidth ||
                    canvas->GetPixelHeight(mip) != pixelHeight)
                {
                    throw love::Exception("All textures must have the same pixel dimensions.");
                }

                if (!multiformatsupported && format != firstColorFormat)
                {
                    throw love::Exception("This system doesn't support multi-render-target "
                                          "rendering with different texture formats.");
                }

                if (canvas->GetRequestedMSAA() != reqMSAA)
                    throw love::Exception("All textures must have the same MSAA value.");

                if (love::IsPixelFormatDepthStencil(format))
                {
                    throw love::Exception(
                        "Depth/stencil format textures must be used with the 'depthstencil' field "
                        "of the table passed into setRenderTargets.");
                }

                if (love::IsPixelFormatSRGB(format))
                    hasSRGBtexture = true;
            }

            /* validate depth stencil */

            this->SetRenderTargetsInternal(targets, pixelWidth, pixelHeight, hasSRGBtexture);

            RenderTargetsStrongReference references {};
            references.colors.reserve(targets.colors.size());

            for (auto target : targets.colors)
                references.colors.emplace_back(target.texture, target.slice, target.mipmap);

            references.depthStencil   = RenderTargetStrongReference(targets.depthStencil.texture,
                                                                    targets.depthStencil.slice);
            references.temporaryFlags = targets.temporaryFlags;

            std::swap(state.renderTargets, references);

            renderTargetSwitchCount++;
        }

        RenderTargets GetRenderTargets() const
        {
            const auto& targets = this->states.back().renderTargets;

            RenderTargets newTargets {};
            newTargets.colors.reserve(targets.colors.size());

            for (const auto& target : targets.colors)
                newTargets.colors.emplace_back(target.texture.Get(), target.slice, target.mipmap);

            newTargets.depthStencil =
                RenderTarget(targets.depthStencil.texture, targets.depthStencil.slice,
                             targets.depthStencil.mipmap);
            newTargets.temporaryFlags = targets.temporaryFlags;

            return newTargets;
        }

        bool IsRenderTargetActive() const
        {
            const auto& targets = this->states.back().renderTargets;
            return !targets.colors.empty() || targets.depthStencil.texture != nullptr;
        }

        bool IsRenderTargetActive(Texture<Console::ALL>* texture) const
        {
            const auto& targets = this->states.back().renderTargets;

            for (const auto& target : targets.colors)
            {
                if (target.texture.Get() == texture)
                    return true;
            }

            if (targets.depthStencil.texture.Get() == texture)
                return true;

            return false;
        }

        bool IsRenderTargetActive(Texture<Console::ALL>* texture, int slice) const
        {
            const auto& targets = this->states.back().renderTargets;

            for (const auto& target : targets.colors)
            {
                if (target.texture.Get() == texture && target.slice == slice)
                    return true;
            }

            if (targets.depthStencil.texture.Get() == texture &&
                targets.depthStencil.slice == slice)
            {
                return true;
            }

            return false;
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

        Font* NewDefaultFont(int size, TrueTypeRasterizer<>::Hinting hinting) const
        {
            auto module = Module::GetInstance<FontModule<Console::ALL>>(M_FONT);

            if (!module)
                throw love::Exception("Font module has not been loaded.");

            StrongRasterizer rasterizer(module->NewTrueTypeRasterizer(size, hinting));
            return this->NewFont(rasterizer.Get());
        }

        void CheckSetDefaultFont()
        {
            if (this->states.back().font.Get() != nullptr)
                return;

            if (!this->defaultFont.Get())
            {
                this->defaultFont.Set(
                    this->NewDefaultFont(13, TrueTypeRasterizer<>::HINTING_NORMAL));
            }

            this->states.back().font.Set(this->defaultFont.Get());
        }

        void Print(const ColoredStrings& strings, const Matrix4& matrix)
        {
            this->CheckSetDefaultFont();

            if (this->states.back().font.Get() != nullptr)
                this->Print(strings, this->states.back().font.Get(), matrix);
        }

        void Print(const ColoredStrings& strings, Font* font, const Matrix4& matrix)
        {
            font->Print(*this, strings, matrix, this->states.back().foreground);
        }

        void Printf(const ColoredStrings& strings, float wrap, Font::AlignMode align,
                    const Matrix4& matrix)
        {
            this->CheckSetDefaultFont();

            if (this->states.back().font.Get() != nullptr)
                this->Printf(strings, this->states.back().font.Get(), wrap, align, matrix);
        }

        void Printf(const ColoredStrings& strings, Font* font, float wrap, Font::AlignMode align,
                    const Matrix4& matrix)
        {
            font->Printf(*this, strings, wrap, align, matrix, this->states.back().foreground);
        }

        Shader<Console::Which>* GetShader() const
        {
            return this->states.back().shader.Get();
        }

        Stats GetStats() const
        {
            Stats stats {};

            stats.drawCalls            = Renderer<>::drawCalls;
            stats.textures             = Texture<>::textureCount;
            stats.fonts                = Font::fontCount;
            stats.shaderSwitches       = Shader<>::shaderSwitches;
            stats.textureMemory        = Texture<>::totalGraphicsMemory;
            stats.drawCallsBatched     = Renderer<>::drawCallsBatched;
            stats.renderTargetSwitches = renderTargetSwitchCount;

            stats.cpuTime = Renderer<>::cpuTime;
            stats.gpuTime = Renderer<>::gpuTime;

            return stats;
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

        void PushTransform()
        {
            this->transformStack.push_back(this->transformStack.back());
        }

        void PushIdentityTransform()
        {
            this->transformStack.push_back(Matrix4());
        }

        void PopTransform()
        {
            this->transformStack.pop_back();
        }

        const Matrix4& GetTransform() const
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

        void ApplyTransform(const Matrix4& matrix)
        {
            auto& current = this->transformStack.back();
            current *= matrix;

            float scaleX, scaleY;
            current.GetApproximateScale(scaleX, scaleY);
            this->pixelScaleStack.back() = (scaleX + scaleY) / 2.0;
        }

        void ReplaceTransform(const Matrix4& matrix)
        {
            this->transformStack.back() = matrix;

            float scaleX, scaleY;
            matrix.GetApproximateScale(scaleX, scaleY);
            this->pixelScaleStack.back() = (scaleX + scaleY) / 2.0;
        }

        Vector2 TransformPoint(Vector2 point)
        {
            Vector2 result {};
            this->transformStack.back().TransformXY(std::views::single(result),
                                                    std::views::single(point));

            return result;
        }

        Vector2 InverseTransformPoint(Vector2 point)
        {
            Vector2 result {};
            this->transformStack.back().Inverse().TransformXY(std::views::single(result),
                                                              std::views::single(point));

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

            this->SetBlendState(state.blendState);

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

            this->SetFont(state.font.Get());
            this->SetShader(state.shader.Get());
            this->SetRenderTargets(state.renderTargets);

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

        void RestoreStateChecked(const DisplayState& state)
        {
            const DisplayState& current = this->states.back();

            if (state.foreground != current.foreground)
                this->SetColor(state.foreground);

            this->SetBackgroundColor(state.background);

            if (!(state.blendState == current.blendState))
                this->SetBlendState(state.blendState);

            this->SetLineWidth(state.line.width);
            this->SetLineStyle(state.line.style);
            this->SetLineJoin(state.line.join);

            if (state.pointSize != current.pointSize)
                this->SetPointSize(state.pointSize);

            bool scissorActive     = state.scissor.active != current.scissor.active;
            bool sameScissorBounds = state.scissor.bounds == current.scissor.bounds;

            if (scissorActive || (state.scissor.active && !sameScissorBounds))
            {
                if (state.scissor.active)
                    this->SetScissor(state.scissor.bounds);
                else
                    this->SetScissor();
            }

            this->SetMeshCullMode(state.cullMode);

            if (state.windingMode != current.windingMode)
                this->SetFrontFaceWinding(state.windingMode);

            this->SetFont(state.font.Get());
            this->SetShader(state.shader.Get());

            const auto& targets        = state.renderTargets;
            const auto& currentTargets = current.renderTargets;

            bool changed = targets.colors.size() != currentTargets.colors.size();

            if (!changed)
            {
                for (size_t index = 0;
                     index < targets.colors.size() && index < currentTargets.colors.size(); index++)
                {
                    if (targets.colors[index] != currentTargets.colors[index])
                    {
                        changed = true;
                        break;
                    }
                }

                if (!changed && targets.depthStencil != currentTargets.depthStencil)
                    changed = true;

                if (targets.temporaryFlags != currentTargets.temporaryFlags)
                    changed = true;
            }

            if (changed)
                this->SetRenderTargets(state.renderTargets);

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
            Renderer<Console::Which>::Instance().SetLineWidth(width);
        }

        const float GetLineWidth() const
        {
            return this->states.back().line.width;
        }

        void SetLineStyle(RenderState::LineStyle style)
        {
            this->states.back().line.style = style;
            Renderer<Console::Which>::Instance().SetLineStyle(style);
        }

        const RenderState::LineStyle GetLineStyle() const
        {
            return this->states.back().line.style;
        }

        void SetLineJoin(RenderState::LineJoin join)
        {
            this->states.back().line.join = join;
        }

        const RenderState::LineJoin GetLineJoin() const
        {
            return this->states.back().line.join;
        }

        void SetPointSize(float size)
        {
            this->states.back().pointSize = size;
            Renderer<Console::Which>::Instance().SetPointSize(size);
        }

        const float GetPointSize() const
        {
            return this->states.back().pointSize;
        }

        void SetScissor(const Rect& scissor)
        {
            this->states.back().scissor.bounds = scissor;
            this->states.back().scissor.active = true;
            Renderer<Console::Which>::Instance().SetScissor(scissor, this->IsRenderTargetActive());
        }

        void SetScissor()
        {
            this->states.back().scissor.active = false;
            Renderer<Console::Which>::Instance().SetScissor(Rect::EMPTY, false);
        }

        bool GetScissor(Rect& rectangle) const
        {
            const auto& state = this->states.back().scissor;
            rectangle         = state.bounds;

            return state.active;
        }

        void SetMeshCullMode(vertex::CullMode mode)
        {
            this->states.back().cullMode = mode;
            Renderer<Console::Which>::Instance().SetMeshCullMode(mode);
        }

        const vertex::CullMode GetMeshCullMode() const
        {
            return this->states.back().cullMode;
        }

        void SetFrontFaceWinding(vertex::Winding winding)
        {
            this->states.back().windingMode = winding;
            Renderer<Console::Which>::Instance().SetVertexWinding(winding);
        }

        const vertex::Winding GetFrontFaceWinding() const
        {
            return this->states.back().windingMode;
        }

        void SetColorMask(const RenderState::ColorMask& mask)
        {
            this->states.back().colorMask = mask;
            Renderer<Console::Which>::Instance().SetColorMask(mask);
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

        /* PRIMITIVES */

        void Polyline(const std::span<Vector2> points)
        {
            float halfWidth                  = this->GetLineWidth() * 0.5f;
            RenderState::LineJoin lineJoin   = this->GetLineJoin();
            RenderState::LineStyle lineStyle = this->GetLineStyle();

            float pixelSize   = 1.0f / std::max((float)this->pixelScaleStack.back(), 0.000001f);
            bool shouldSmooth = lineStyle == RenderState::LINE_SMOOTH;

            if (lineJoin == RenderState::LINE_JOIN_NONE)
            {
                NoneJoinPolyline line;
                line.render(points.data(), points.size(), halfWidth, pixelSize, shouldSmooth);

                line.draw(this);
            }
            else if (lineJoin == RenderState::LINE_JOIN_BEVEL)
            {
                BevelJoinPolyline line;
                line.render(points.data(), points.size(), halfWidth, pixelSize, shouldSmooth);

                line.draw(this);
            }
            else if (lineJoin == RenderState::LINE_JOIN_MITER)
            {
                MiterJoinPolyline line;
                line.render(points.data(), points.size(), halfWidth, pixelSize, shouldSmooth);

                line.draw(this);
            }
        }

        void Polygon(DrawMode mode, std::span<Vector2> points, bool skipLastVertex = true)
        {
            if (mode == DRAW_LINE)
                this->Polyline(points);
            else
            {
                const auto transform = this->GetTransform();
                bool is2D            = transform.IsAffine2DTransform();

                const int count = points.size() - (skipLastVertex ? 1 : 0);
                DrawCommand command(count, vertex::PRIMITIVE_TRIANGLE_FAN);

                if (is2D)
                    transform.TransformXY(std::span(command.Positions().get(), command.count),
                                          points);

                command.FillVertices(this->GetColor());

                Renderer<Console::Which>::Instance().Render(command);
            }
        }

        int CalculateEllipsePoints(float rx, float ry) const
        {
            auto pixelScale = (float)this->pixelScaleStack.back();
            auto points     = sqrtf(((rx + ry) / 2.0f) * 20.0f * pixelScale);

            return std::max(points, 8.0f);
        }

        void Rectangle(DrawMode mode, float x, float y, float width, float height)
        {
            std::array<Vector2, 0x05> points = { Vector2(x, y), Vector2(x, y + height),
                                                 Vector2(x + width, y + height),
                                                 Vector2(x + width, y), Vector2(x, y) };

            this->Polygon(mode, points);
        }

        void Rectangle(DrawMode mode, float x, float y, float width, float height, float rx,
                       float ry, int points)
        {
            if (rx <= 0 || ry <= 0)
            {
                this->Rectangle(mode, x, y, width, height);
                return;
            }

            if (width >= 0.02f)
                rx = std::min(rx, width / 2.0f - 0.01f);

            if (height >= 0.02f)
                ry = std::min(ry, height / 2.0f - 0.01f);

            points = std::max(points / 4, 1);

            const float halfPi = static_cast<float>(LOVE_M_PI / 2);
            float angleShift   = halfPi / ((float)points + 1.0f);

            int pointCount = (points + 2) * 4;

            Vector2 coords[pointCount + 1] {};
            float phi = 0.0f;

            for (int index = 0; index <= points + 2; ++index, phi += angleShift)
            {
                coords[index].x = x + rx * (1 - cosf(phi));
                coords[index].y = y + ry * (1 - sinf(phi));
            }

            phi = halfPi;

            for (int index = points + 2; index <= 2 * (points + 2); ++index, phi += angleShift)
            {
                coords[index].x = x + width - rx * (1 + cosf(phi));
                coords[index].y = y + ry * (1 - sinf(phi));
            }

            phi = 2 * halfPi;

            for (int index = 2 * (points + 2); index <= 3 * (points + 2);
                 ++index, phi += angleShift)
            {
                coords[index].x = x + width - rx * (1 + cosf(phi));
                coords[index].y = y + height - ry * (1 + sinf(phi));
            }

            phi = 3 * halfPi;

            for (int index = 3 * (points + 2); index <= 4 * (points + 2);
                 ++index, phi += angleShift)
            {
                coords[index].x = x + rx * (1 - cosf(phi));
                coords[index].y = y + height - ry * (1 + sinf(phi));
            }

            coords[pointCount] = coords[0];
            this->Polygon(mode, std::span(coords, pointCount + 1));
        }

        void Rectangle(DrawMode mode, float x, float y, float width, float height, float rx,
                       float ry)
        {
            const float pointsRx = std::min(rx, std::abs(width / 2));
            const float pointsRy = std::min(ry, std::abs(height / 2));

            int points = this->CalculateEllipsePoints(pointsRx, pointsRy);
            this->Rectangle(mode, x, y, width, height, rx, ry, points);
        }

        void Ellipse(DrawMode mode, float x, float y, float a, float b, int points)
        {
            float twoPi = (float)(LOVE_M_TAU);

            if (points <= 0)
                points = 1;

            const float angleShift = (twoPi / points);
            float phi              = 0.0f;

            int extraPoints = 1 + (mode == DRAW_FILL ? 1 : 0);
            Vector2 polygonCoords[points + extraPoints] {};
            Vector2* coords = polygonCoords;

            if (mode == DRAW_FILL)
            {
                coords[0].x = x;
                coords[0].y = y;
                coords++;
            }

            for (int index = 0; index < points; ++index, phi += angleShift)
            {
                coords[index].x = x + a * cosf(phi);
                coords[index].y = y + b * sinf(phi);
            }

            coords[points] = coords[0];

            this->Polygon(mode, std::span(polygonCoords, points + extraPoints), false);
        }

        void Ellipse(DrawMode mode, float x, float y, float a, float b)
        {
            this->Ellipse(mode, x, y, a, b, this->CalculateEllipsePoints(a, b));
        }

        void Circle(DrawMode mode, float x, float y, float radius, int points)
        {
            this->Ellipse(mode, x, y, radius, radius, points);
        }

        void Circle(DrawMode mode, float x, float y, float radius)
        {
            this->Ellipse(mode, x, y, radius, radius);
        }

        void Arc(DrawMode mode, ArcMode arcMode, float x, float y, float radius, float angle1,
                 float angle2, int points)
        {
            if (points <= 0 || angle1 == angle2)
                return;

            if (fabs(angle1 - angle2) >= LOVE_M_TAU)
            {
                this->Circle(mode, x, y, radius, points);
                return;
            }

            const float angleShift = (angle2 - angle1) / points;

            if (angleShift == 0.0f)
                return;

            const auto sharpAngle = fabsf(angle1 - angle2) < LOVE_TORAD(4);
            if (mode == DRAW_LINE && arcMode == ARC_CLOSED && sharpAngle)
                arcMode = ARC_OPEN;

            if (mode == DRAW_FILL && arcMode == ARC_OPEN)
                arcMode = ARC_CLOSED;

            float phi = angle1;

            Vector2* coords = nullptr;
            int numCoords   = 0;

            // clang-format off
            const auto createPoints = [&](Vector2* coordinates)
            {
                for (int index = 0; index <= points; ++index, phi += angleShift)
                {
                    coordinates[index].x = x + radius * cosf(phi);
                    coordinates[index].y = y + radius * sinf(phi);
                }
            };
            // clang-format on

            if (arcMode == ARC_PIE)
            {
                numCoords = points + 3;
                coords    = new Vector2[numCoords];

                coords[0] = coords[numCoords - 1] = Vector2(x, y);
                createPoints(coords + 1);
            }
            else if (arcMode == ARC_OPEN)
            {
                numCoords = points + 1;
                coords    = new Vector2[numCoords];

                createPoints(coords);
            }
            else
            {
                numCoords = points + 2;
                coords    = new Vector2[numCoords];

                createPoints(coords);
                coords[numCoords - 1] = coords[0];
            }

            this->Polygon(mode, std::span(coords, numCoords));
            delete[] coords;
        }

        void Arc(DrawMode mode, ArcMode arcMode, float x, float y, float radius, float angle1,
                 float angle2)
        {
            float points = this->CalculateEllipsePoints(radius, radius);
            float angle  = fabsf(angle1 - angle2);

            if (angle < (float)LOVE_M_TAU)
                points *= angle / (float)LOVE_M_TAU;

            this->Arc(mode, arcMode, x, y, radius, angle1, angle2, (int)(points + 0.5f));
        }

        void Points(std::span<Vector2> points, std::span<Color> colors)
        {
            const auto& transform = this->GetTransform();
            bool is2D             = transform.IsAffine2DTransform();

            DrawCommand command(points.size(), vertex::PRIMITIVE_POINTS);

            if (is2D)
                transform.TransformXY(std::span(command.Positions().get(), points.size()), points);

            if (colors.size() > 1)
                command.FillVertices(colors);
            else
                command.FillVertices(colors[0]);

            Renderer<Console::Which>::Instance().Render(command);
        }

        void Line(std::span<Vector2> points)
        {
            this->Polyline(points);
        }

        /* PRIMITIVES */

        void SetBlendState(const RenderState::BlendState& state)
        {
            Renderer<Console::Which>::Instance().SetBlendMode(state);
            this->states.back().blendState = state;
        }

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

            const auto state = RenderState::ComputeBlendState(mode, alphaMode);
            Renderer<Console::Which>::Instance().SetBlendMode(state);
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

        void InternalScale(const Matrix4& transform)
        {
            this->transformStack.back() *= transform;
        }

        void IntersectScissor(const Rect& rectangle)
        {
            Rect currect = states.back().scissor.bounds;

            if (!states.back().scissor.active)
            {
                currect.x = 0;
                currect.y = 0;
                currect.w = std::numeric_limits<int>::max();
                currect.h = std::numeric_limits<int>::max();
            }

            int x1 = std::max(currect.x, rectangle.x);
            int y1 = std::max(currect.y, rectangle.y);

            int x2 = std::min(currect.x + currect.w, rectangle.x + rectangle.w);
            int y2 = std::min(currect.y + currect.h, rectangle.y + rectangle.h);

            Rect newrect = { x1, y1, std::max(0, x2 - x1), std::max(0, y2 - y1) };
            SetScissor(newrect);
        }

        Quad* NewQuad(const Quad::Viewport& viewport, double sourceWidth, double sourceHeight) const
        {
            return new Quad(viewport, sourceWidth, sourceHeight);
        }

        // clang-format off
        static constexpr BidirectionalMap drawModes = {
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
        // clang-format on

      protected:
        std::vector<double> pixelScaleStack;
        std::vector<Matrix4> transformStack;

        std::vector<DisplayState> states;
        std::vector<StackType> stackTypeStack;

        int width;
        int height;

        int pixelWidth;
        int pixelHeight;

        bool created;
        bool active;

        int renderTargetSwitchCount;

        StrongReference<Font> defaultFont;
    };
} // namespace love
