#pragma once

#include "common/Color.hpp"
#include "common/Map.hpp"
#include "common/Matrix.hpp"
#include "common/Module.hpp"
#include "common/Optional.hpp"
#include "common/Vector.hpp"
#include "common/int.hpp"
#include "common/pixelformat.hpp"

#include "modules/math/MathModule.hpp"

#include "modules/graphics/Shader.tcc"
#include "modules/graphics/Texture.tcc"
#include "modules/graphics/Volatile.hpp"
#include "modules/graphics/renderstate.hpp"
#include "modules/graphics/samplerstate.hpp"

#include "driver/graphics/DrawCommand.hpp"

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

    inline void gammaCorrectColor(Color& color)
    {
        if (love::isGammaCorrect())
        {
            color.r = love::gammaToLinear(color.r);
            color.g = love::gammaToLinear(color.g);
            color.b = love::gammaToLinear(color.b);
        }
    }

    inline void unGammaCorrectColor(Color& color)
    {
        if (love::isGammaCorrect())
        {
            color.r = love::linearToGamma(color.r);
            color.g = love::linearToGamma(color.g);
            color.b = love::linearToGamma(color.b);
        }
    }

    inline Color gammaCorrectColor(const Color& color)
    {
        Color result = color;
        gammaCorrectColor(result);

        return result;
    }

    inline Color unGammaCorrectColor(const Color& color)
    {
        Color result = color;
        unGammaCorrectColor(result);

        return result;
    }

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

            SamplerState defaultSampleState = SamplerState();
        };

        class TempTransform
        {
          public:
            TempTransform(GraphicsBase* graphics) : graphics(graphics)
            {
                graphics->pushTransform();
            }

            TempTransform(GraphicsBase* graphics, const Matrix4& transform) : graphics(graphics)
            {
                graphics->pushTransform();
                graphics->transformStack.back() *= transform;
            }

            ~TempTransform()
            {
                graphics->popTransform();
            }

          private:
            GraphicsBase* graphics;
        };

        GraphicsBase(const char* name);

        virtual ~GraphicsBase();

        void restoreState(const DisplayState& state);

        void restoreStateChecked(const DisplayState& state);

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

        const SamplerState& getDefaultSamplerState() const
        {
            return this->states.back().defaultSampleState;
        }

        void setDefaultSamplerState(const SamplerState& state)
        {
            this->states.back().defaultSampleState = state;
        }

        void setScissor(const Rect& scissor);

        void setScissor();

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

        void setFrontFaceWinding(Winding winding);

        Winding getFrontFaceWinding() const;

        void setColorMask(ColorChannelMask mask);

        ColorChannelMask getColorMask() const;

        void setBlendMode(BlendMode mode, BlendAlpha alphaMode);

        BlendMode getBlendMode(BlendAlpha& alphaMode) const
        {
            return computeBlendMode(this->states.back().blend, alphaMode);
        }

        void setBlendState(const BlendState& blend);

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

        RendererInfo getRendererInfo() const
        {

            RendererInfo info { .name    = __RENDERER_NAME__,
                                .version = __RENDERER_VERSION__,
                                .vendor  = __RENDERER_VENDOR__,
                                .device  = __RENDERER_DEVICE__ };

            return info;
        }

        BatchedVertexData requestBatchDraw(const DrawCommand& command);

        void flushBatchedDraws();

        static void flushBatchedDrawsGlobal();

        virtual void draw(const DrawIndexedCommand& command) = 0;

        Stats getStats() const;

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

        double getCurrentDPIScale()
        {
            return 1.0;
        }

        double getScreenDPIScale()
        {
            return 1.0;
        }

        void polyline(const std::span<Vector2> vertices);

        void polygon(DrawMode mode, const std::span<Vector2> vertices, bool skipLastVertex = true);

        void rectangle(DrawMode mode, float x, float y, float w, float h);

        void rectangle(DrawMode mode, float x, float y, float w, float h, float rx, float ry, int points);

        void rectangle(DrawMode mode, float x, float y, float w, float h, float rx, float ry);

        void circle(DrawMode mode, float x, float y, float radius, int points);

        void circle(DrawMode mode, float x, float y, float radius);

        void ellipse(DrawMode mode, float x, float y, float a, float b, int points);

        void ellipse(DrawMode mode, float x, float y, float a, float b);

        void arc(DrawMode mode, ArcMode arcMode, float x, float y, float radius, float angle1, float angle2,
                 int points);

        void arc(DrawMode mode, ArcMode arcMode, float x, float y, float radius, float angle1, float angle2);

        void points(const Vector2* points, const Color* colors, int count);

        template<typename T>
        T* getScratchBuffer(size_t count)
        {
            size_t bytes = count * sizeof(T);

            if (this->scratchBuffer.size() < bytes)
                this->scratchBuffer.resize(bytes);

            return (T*)this->scratchBuffer.data();
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

      private:
        int calculateEllipsePoints(float a, float b) const;

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

        int drawCallsBatched;
        int drawCalls;

        BatchedDrawState batchedDrawState;
        std::vector<uint8_t> scratchBuffer;
    };
} // namespace love
