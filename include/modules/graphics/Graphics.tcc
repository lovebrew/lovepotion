#pragma once

#include "common/Color.hpp"
#include "common/Map.hpp"
#include "common/Matrix.hpp"
#include "common/Module.hpp"
#include "common/Optional.hpp"
#include "common/Vector.hpp"
#include "common/int.hpp"
#include "common/pixelformat.hpp"

#include "modules/graphics/Volatile.hpp"
#include "modules/graphics/renderstate.hpp"

#include <string>
#include <vector>

namespace love
{
    class SpriteBatch;
    class ParticleSystem;
    class TextBatch;
    class Video;
    class Buffer;

    using OptionalColor = Optional<Color>;

    static bool gammaCorrect = false;

    inline void setGammaCorrect(bool enable)
    {
        gammaCorrect = enable;
    }

    inline bool isGammaCorrect()
    {
        return gammaCorrect;
    }

    void gammaCorrectColor(Color& color);

    void unGammaCorrectColor(Color& color);

    Color gammaCorrectColor(const Color& color);

    Color unGammaCorrectColor(const Color& color);

    template<class T>
    class GraphicsBase : public Module
    {
      public:
        static constexpr size_t MAX_USER_STACK_DEPTH = 128;

        enum DrawMode
        {
            DRAW_LINE,
            DRAW_FILL,
            DRAW_MAX_ENUM
        };

        enum ArcMode
        {
            ARC_OPEN,
            ARC_CLOSED,
            ARC_PIE,
            ARC_MAX_ENUM
        };

        enum LineStyle
        {
            LINE_SMOOTH,
            LINE_ROUGH,
            LINE_MAX_ENUM
        };

        enum LineJoin
        {
            LINE_JOIN_NONE,
            LINE_JOIN_MITER,
            LINE_JOIN_BEVEL,
            LINE_JOIN_MAX_ENUM
        };

        enum StackType
        {
            STACK_ALL,
            STACK_TRANSFORM,
            STACK_MAX_ENUM
        };

        enum TemporaryRenderTargetFlags
        {
            TEMPORARY_RT_DEPTH   = (1 << 0),
            TEMPORARY_RT_STENCIL = (1 << 1)
        };

        struct RendererInfo
        {
            std::string name;
            std::string version;
            std::string vendor;
            std::string device;
        };

        struct Stats
        {
            int drawCalls;
            int drawCallsBatched;
            int renderTargetSwitches;
            int shaderSwitches;
            int textures;
            int fonts;
            int buffers;
            int64_t textureMemory;
            int64_t bufferMemory;
        };

        struct DisplayState
        {
            DisplayState()
            {}

            Color color           = Color::WHITE;
            Color backgroundColor = Color::BLACK;

            BlendState blend = computeBlendState(BLEND_ALPHA, BLENDALPHA_MULTIPLY);

            float lineWidth     = 1.0f;
            LineStyle lineStyle = LINE_SMOOTH;
            LineJoin lineJoin   = LINE_JOIN_MITER;

            float pointSize = 1.0f;

            bool scissor     = false;
            Rect scissorRect = Rect();

            StencilState stencil;

            CompareMode depthTest = COMPARE_ALWAYS;
            bool depthWrite       = false;

            CullMode meshCullMode = CULL_NONE;
            Winding winding       = WINDING_CCW;

            // StrongRef<Font> font;
            // StrongRef<Shader> shader;

            // RenderTargetsStrongRef renderTargets;

            ColorChannelMask colorMask;

            bool useCustomProjection = false;
            Matrix4 customProjection;

            // SamplerState defaultSampleState = SamplerState();
        };

        GraphicsBase(const char* name) :
            Module(M_GRAPHICS, name),
            width(0),
            height(0),
            pixelWidth(0),
            pixelHeight(0),
            created(false),
            active(true),
            deviceProjectionMatrix()
        {
            this->transformStack.reserve(16);
            this->transformStack.push_back(Matrix4());

            this->pixelScaleStack.reserve(16);
            this->pixelScaleStack.push_back(1.0);

            this->states.reserve(10);
            this->states.push_back(DisplayState());
        }

        virtual ~GraphicsBase()
        {
            this->states.clear();
        }

        void restoreState(const DisplayState& state)
        {
            this->setColor(state.color);
            this->setBackgroundColor(state.backgroundColor);

            this->setBlendState(state.blend);

            this->setLineWidth(state.lineWidth);
            this->setLineStyle(state.lineStyle);
            this->setLineJoin(state.lineJoin);

            this->setPointSize(state.pointSize);

            if (state.scissor)
                this->setScissor(state.scissorRect);
            else
                this->setScissor();

            this->setMeshCullMode(state.meshCullMode);
            this->setFrontFaceWinding(state.winding);

            // this->setFont(state.font.get());
            // this->setShader(state.shader.get());
            // this->setRenderTargets(state.renderTargets);

            // this->setStencilState(state.stencil);
            // this->setDepthMode(state.depthTest, state.depthWrite);

            this->setColorMask(state.colorMask);

            // this->setDefaultSamplerState(state.defaultSampleState);

            if (state.useCustomProjection)
                this->updateDeviceProjection(state.customProjection);
            else
                this->resetProjection();
        }

        void restoreStateChecked(const DisplayState& state)
        {
            const auto& current = this->states.back();

            if (state.color != current.color)
                this->setColor(state.color);

            this->setBackgroundColor(state.backgroundColor);

            if (state.blend != current.blend)
                this->setBlendState(state.blend);

            this->setLineWidth(state.lineWidth);
            this->setLineStyle(state.lineStyle);
            this->setLineJoin(state.lineJoin);

            if (state.pointSize != current.pointSize)
                this->setPointSize(state.pointSize);

            if (state.scissor != current.scissor ||
                (state.scissor && !(state.scissorRect != current.scissorRect)))
            {
                if (state.scissor)
                    this->setScissor(state.scissorRect);
                else
                    this->setScissor();
            }

            this->setMeshCullMode(state.meshCullMode);

            if (state.winding != current.winding)
                this->setFrontFaceWinding(state.winding);

            // this->setFont(state.font.get());
            // this->setShader(state.shader.get());

            // if (this->stencil != state.stencil)
            //     this->setStencilState(state.stencil);

            // if (this->depthTest != state.depthTest || this->depthWrite != state.depthWrite)
            //     this->setDepthMode(state.depthTest, state.depthWrite);

            if (state.colorMask != current.colorMask)
                this->setColorMask(state.colorMask);

            // this->setDefaultSamplerState(state.defaultSampleState);

            if (state.useCustomProjection)
                this->updateDeviceProjection(state.customProjection);
            else if (current.useCustomProjection)
                this->resetProjection();
        }

        /* TODO: implement when they exist */
        bool isRenderTargetActive() const
        {
            return false;
        }

        void setActive(bool active)
        {
            this->active = active;
        }

        bool isCreated() const
        {
            return this->created;
        }

        void setColor(Color color)
        {
            this->states.back().color = color;
        }

        Color getColor() const
        {
            return this->states.back().color;
        }

        void setBackgroundColor(Color color)
        {
            this->states.back().backgroundColor = color;
        }

        void setScissor(const Rect& scissor)
        {
            auto& state = this->states.back();

            Rect rect {};
            rect.x = scissor.x;
            rect.y = scissor.y;
            rect.w = std::max(0, scissor.w);
            rect.h = std::max(0, scissor.h);

            static_cast<T*>(this)->setScissorImpl(scissor);

            state.scissorRect = rect;
            state.scissor     = true;
        }

        void setScissor()
        {
            this->states.back().scissor = false;
            static_cast<T*>(this)->setScissorImpl();
        }

        void intersectScissor(const Rect& scissor)
        {
            auto current = this->states.back().scissorRect;

            if (!this->states.back().scissor)
            {
                current.x = 0;
                current.y = 0;

                current.w = std::numeric_limits<int>::max();
                current.h = std::numeric_limits<int>::max();
            }

            int x1 = std::max(current.x, scissor.x);
            int y1 = std::max(current.y, scissor.y);
            int x2 = std::min(current.x + current.w, scissor.x + scissor.w);
            int y2 = std::min(current.y + current.h, scissor.y + scissor.h);

            Rect newScisssor = { x1, y1, std::max(0, x2 - x1), std::max(0, y2 - y1) };
            this->setScissor(newScisssor);
        }

        bool getScissor(Rect& scissor) const
        {
            const auto& state = this->states.back();

            scissor = state.scissorRect;
            return state.scissor;
        }

        Color getBackgroundColor() const
        {
            return this->states.back().backgroundColor;
        }

        void setMeshCullMode(CullMode mode)
        {
            this->states.back().meshCullMode = mode;
        }

        CullMode getMeshCullMode() const
        {
            return this->states.back().meshCullMode;
        }

        void setFrontFaceWinding(Winding winding)
        {
            static_cast<T*>(this)->setFrontFaceWindingImpl(winding);
            this->states.back().winding = winding;
        }

        Winding getFrontFaceWinding() const
        {
            return this->states.back().winding;
        }

        void setColorMask(ColorChannelMask mask)
        {
            static_cast<T*>(this)->setColorMaskImpl(mask);
            this->states.back().colorMask = mask;
        }

        ColorChannelMask getColorMask() const
        {
            return this->states.back().colorMask;
        }

        void setBlendMode(BlendMode mode, BlendAlpha alphaMode)
        {
            if (alphaMode == BLENDALPHA_MULTIPLY && !isAlphaMultiplyBlendSupported(mode))
            {
                std::string_view modeName = "unknown";
                love::getConstant(mode, modeName);

                throw love::Exception("The '{}' blend mode must be used with premultiplied alpha.",
                                      modeName);
            }

            this->setBlendState(computeBlendState(mode, alphaMode));
        }

        BlendMode getBlendMode(BlendAlpha& alphaMode) const
        {
            return computeBlendMode(this->states.back().blend, alphaMode);
        }

        void setBlendState(const BlendState& blend)
        {
            static_cast<T*>(this)->setBlendStateImpl(blend);
            this->states.back().blend = blend;
        }

        const BlendState& getBlendState() const
        {
            return this->states.back().blend;
        }

        void setLineWidth(float width)
        {
            this->states.back().lineWidth = width;
        }

        float getLineWidth() const
        {
            return this->states.back().lineWidth;
        }

        void setLineStyle(LineStyle style)
        {
            this->states.back().lineStyle = style;
        }

        LineStyle getLineStyle() const
        {
            return this->states.back().lineStyle;
        }

        void setLineJoin(LineJoin join)
        {
            this->states.back().lineJoin = join;
        }

        LineJoin getLineJoin() const
        {
            return this->states.back().lineJoin;
        }

        void setPointSize(float size)
        {
            this->states.back().pointSize = size;
        }

        float getPointSize() const
        {
            return this->states.back().pointSize;
        }

        PixelFormat getSizedFormat(PixelFormat format)
        {
            switch (format)
            {
                case PIXELFORMAT_NORMAL:
                    if (isGammaCorrect())
                        return PIXELFORMAT_RGBA8_sRGB;
                    else
                        return PIXELFORMAT_RGBA8_UNORM;
                case PIXELFORMAT_HDR:
                    return PIXELFORMAT_RGBA16_FLOAT;
                default:
                    return format;
            }
        }

        // bool isPixelFormatSupported(PixelFormat format, uint32_t usage)
        // {
        //     format = getSizedFormat(format);

        //     bool readable = (usage & PIXELFORMATUSAGEFLAGS_SAMPLE) != 0;
        //     return (usage & pixelFormatUsage[format][readable ? 1 : 0]) == usage;
        // }

        RendererInfo getRendererInfo() const
        {

            RendererInfo info { .name    = __RENDERER_NAME__,
                                .version = __RENDERER_VERSION__,
                                .vendor  = __RENDERER_VENDOR__,
                                .device  = __RENDERER_DEVICE__ };

            return info;
        }

        Stats getStats() const
        {
            Stats stats {};

            return stats;
        }

        size_t getStackDepth() const
        {
            return this->stackTypeStack.size();
        }

        void push(StackType type = STACK_TRANSFORM)
        {
            if (this->getStackDepth() == MAX_USER_STACK_DEPTH)
                throw love::Exception("Maximum stack depth reached (more pushes than pops?)");

            this->pushTransform();
            this->pixelScaleStack.push_back(this->pixelScaleStack.back());

            if (type == STACK_ALL)
                this->states.push_back(this->states.back());

            this->stackTypeStack.push_back(type);
        }

        void pop()
        {
            if (this->getStackDepth() < 1)
                throw love::Exception("Minimum stack depth reached (more pops than pushes?)");

            this->popTransform();
            this->pixelScaleStack.pop_back();

            if (this->stackTypeStack.back() == STACK_ALL)
            {
                DisplayState state = this->states[this->states.size() - 2];
                this->restoreStateChecked(state);

                this->states.pop_back();
            }

            this->stackTypeStack.pop_back();
        }

        const Matrix4& getTransform() const
        {
            return this->transformStack.back();
        }

        void pushTransform()
        {
            this->transformStack.push_back(this->transformStack.back());
        }

        void pushIdentityTransform()
        {
            this->transformStack.push_back(Matrix4());
        }

        void popTransform()
        {
            this->transformStack.pop_back();
        }

        void rotate(float angle)
        {
            this->transformStack.back().rotate(angle);
        }

        void scale(float sx, float sy)
        {
            this->transformStack.back().scale(sx, sy);
            this->pixelScaleStack.back() *= (std::fabs(sx) + std::fabs(sy)) / 2.0;
        }

        void translate(float dx, float dy)
        {
            this->transformStack.back().translate(dx, dy);
        }

        void shear(float kx, float ky)
        {
            this->transformStack.back().shear(kx, ky);
        }

        void origin()
        {
            this->transformStack.back().setIdentity();
            this->pixelScaleStack.back() = 1.0;
        }

        void applyTransform(const Matrix4& transform)
        {
            Matrix4& current = this->transformStack.back();
            current *= transform;

            float sx, sy;
            current.getApproximateScale(sx, sy);
            this->pixelScaleStack.back() *= (sx + sy) / 2.0;
        }

        void replaceTransform(const Matrix4& transform)
        {
            this->transformStack.back() = transform;

            float sx, sy;
            transform.getApproximateScale(sx, sy);
            this->pixelScaleStack.back() = (sx + sy) / 2.0;
        }

        Vector2 transformPoint(Vector2 point) const
        {
            Vector2 result {};
            this->transformStack.back().transformXY(&result, &point, 1);

            return result;
        }

        Vector2 inverseTransformPoint(Vector2 point) const
        {
            Vector2 result {};
            this->transformStack.back().inverse().transformXY(&result, &point, 1);

            return result;
        }

        void updateDeviceProjection(const Matrix4& projection)
        {
            this->deviceProjectionMatrix = projection;
        }

        const Matrix4& getDeviceProjection() const
        {
            return this->deviceProjectionMatrix;
        }

        void resetProjection()
        {
            auto& state = this->states.back();

            state.useCustomProjection = false;
            this->updateDeviceProjection(Matrix4::ortho(0.0f, 0, 0, 0.0f, -10.0f, 10.0f));
        }

        void reset()
        {
            DisplayState state {};
            this->restoreState(state);

            this->origin();
        }

        void clear(OptionalColor color, OptionalInt stencil, OptionalDouble depth)
        {
            static_cast<T*>(this)->clearImpl(color, stencil, depth);
        }

        void clear(const std::vector<OptionalColor>& colors, OptionalInt stencil,
                   OptionalDouble depth)
        {
            if (colors.size() == 0 && !stencil.hasValue && !depth.hasValue)
                return;

            const int numColors = (int)colors.size();

            if (numColors <= 1)
            {
                this->clear(colors.size() > 0 ? colors[0] : OptionalColor(), stencil, depth);
                return;
            }
        }

        void present()
        {
            static_cast<T*>(this)->presentImpl();
        }

        bool setMode(int width, int height, int pixelWidth, int pixelHeight, bool backBufferStencil,
                     bool backBufferDepth, int msaa)
        {
            static_cast<T*>(this)->setModeImpl(width, height, pixelWidth, pixelHeight,
                                               backBufferStencil, backBufferDepth, msaa);

            this->created = true;

            if (!Volatile::loadAll())
                std::printf("Failed to load all volatile objects.\n");

            this->restoreState(this->states.back());

            return true;
        }

        void unsetMode()
        {
            static_cast<T*>(this)->unsetModeImpl();
        }

        double getCurrentDPIScale()
        {
            return 1.0;
        }

        double getScreenDPIScale()
        {
            return 1.0;
        }

        // clang-format off
        STRINGMAP_DECLARE(DrawModes, DrawMode,
            { "line", DRAW_LINE },
            { "fill", DRAW_FILL }
        );

        STRINGMAP_DECLARE(ArcModes, ArcMode,
            { "open",   ARC_OPEN   },
            { "closed", ARC_CLOSED },
            { "pie",    ARC_PIE    }
        );

        STRINGMAP_DECLARE(LineStyles, LineStyle,
            { "smooth", LINE_SMOOTH },
            { "rough",  LINE_ROUGH  }
        );

        STRINGMAP_DECLARE(LineJoins, LineJoin,
            { "none",  LINE_JOIN_NONE  },
            { "miter", LINE_JOIN_MITER },
            { "bevel", LINE_JOIN_BEVEL }
        );

        STRINGMAP_DECLARE(StackTypes, StackType,
            { "all",        STACK_ALL        },
            { "transform",  STACK_TRANSFORM  }
        );
        // clang-format on

      protected:
        bool created;
        bool active;

        std::vector<Matrix4> transformStack;
        Matrix4 deviceProjectionMatrix;

        std::vector<double> pixelScaleStack;

        std::vector<DisplayState> states;
        std::vector<StackType> stackTypeStack;

        int width;
        int height;

        int pixelWidth;
        int pixelHeight;
    };
} // namespace love
