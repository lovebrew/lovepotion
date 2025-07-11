#pragma once

#include "common/Color.hpp"
#include "common/Map.hpp"
#include "common/Matrix.hpp"
#include "common/Module.hpp"
#include "common/Optional.hpp"
#include "common/Vector.hpp"
#include "common/int.hpp"
#include "common/pixelformat.hpp"

#include "modules/font/Font.tcc"
#include "modules/math/MathModule.hpp"

#include "modules/graphics/Font.tcc"
#include "modules/graphics/Shader.tcc"
#include "modules/graphics/ShaderStage.tcc"
#include "modules/graphics/TextBatch.hpp"
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
        static constexpr int MAX_VERTICES_PER_DRAW   = LOVE_UINT16_MAX;
        static constexpr int MAX_QUADS_PER_DRAW      = MAX_VERTICES_PER_DRAW / 4;

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

        enum Feature
        {
            FEATURE_MULTI_RENDER_TARGET_FORMATS,
            FEATURE_CLAMP_ZERO,
            FEATURE_CLAMP_ONE,
            FEATURE_BLEND_MINMAX,
            FEATURE_LIGHTEN, // Deprecated
            FEATURE_FULL_NPOT,
            FEATURE_PIXEL_SHADER_HIGHP,
            FEATURE_SHADER_DERIVATIVES,
            FEATURE_GLSL3,
            FEATURE_GLSL4,
            FEATURE_INSTANCING,
            FEATURE_TEXEL_BUFFER,
            FEATURE_INDEX_BUFFER_32BIT,
            FEATURE_COPY_BUFFER,
            FEATURE_COPY_BUFFER_TO_TEXTURE,
            FEATURE_COPY_TEXTURE_TO_BUFFER,
            FEATURE_COPY_RENDER_TARGET_TO_BUFFER,
            FEATURE_MIPMAP_RANGE,
            FEATURE_INDIRECT_DRAW,
            FEATURE_MAX_ENUM
        };

        enum SystemLimit
        {
            LIMIT_POINT_SIZE,
            LIMIT_TEXTURE_SIZE,
            LIMIT_VOLUME_TEXTURE_SIZE,
            LIMIT_CUBE_TEXTURE_SIZE,
            LIMIT_TEXTURE_LAYERS,
            LIMIT_TEXEL_BUFFER_SIZE,
            LIMIT_SHADER_STORAGE_BUFFER_SIZE,
            LIMIT_THREADGROUPS_X,
            LIMIT_THREADGROUPS_Y,
            LIMIT_THREADGROUPS_Z,
            LIMIT_RENDER_TARGETS,
            LIMIT_TEXTURE_MSAA,
            LIMIT_ANISOTROPY,
            LIMIT_MAX_ENUM
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

        struct Capabilities
        {
            double limits[LIMIT_MAX_ENUM];
            bool features[FEATURE_MAX_ENUM];
            bool textureTypes[TEXTURE_MAX_ENUM];
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
            float cpuProcessingTime;
            float gpuDrawingTime;
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

        struct ScreenshotInfo;
        typedef void (*ScreenshotCallback)(const ScreenshotInfo* info, ImageData* i, void* ud);

        struct ScreenshotInfo
        {
            ScreenshotCallback callback = nullptr;
            void* data                  = nullptr;
        };

        struct RenderTargetStrongRef;

        struct RenderTarget
        {
            TextureBase* texture;
            int slice;
            int mipmap;

            RenderTarget(TextureBase* texture, int slice = 0, int mipmap = 0) :
                texture(texture),
                slice(slice),
                mipmap(mipmap)
            {}

            RenderTarget() : texture(nullptr), slice(0), mipmap(0)
            {}

            bool operator!=(const RenderTarget& other) const
            {
                return this->texture != other.texture || this->slice != other.slice ||
                       this->mipmap != other.mipmap;
            }

            bool operator!=(const RenderTargetStrongRef& other) const
            {
                return this->texture != other.texture.get() || this->slice != other.slice ||
                       this->mipmap != other.mipmap;
            }
        };

        struct RenderTargetStrongRef
        {
            StrongRef<TextureBase> texture;
            int slice  = 0;
            int mipmap = 0;

            RenderTargetStrongRef(TextureBase* texture, int slice = 0, int mipmap = 0) :
                texture(texture),
                slice(slice),
                mipmap(mipmap)
            {}

            bool operator!=(const RenderTargetStrongRef& other) const
            {
                return this->texture.get() != other.texture.get() || this->slice != other.slice ||
                       this->mipmap != other.mipmap;
            }

            bool operator!=(const RenderTarget& other)
            {
                return this->texture.get() != other.texture || this->slice != other.slice ||
                       this->mipmap != other.mipmap;
            }
        };

        struct RenderTargetsStrongRef
        {
            std::vector<RenderTargetStrongRef> colors;
            RenderTargetStrongRef depthStencil;
            uint32_t temporaryFlags;

            RenderTargetsStrongRef() : depthStencil(nullptr), temporaryFlags(0)
            {}
        };

        struct RenderTargets
        {
            std::vector<RenderTarget> colors;
            RenderTarget depthStencil;
            uint32_t temporaryFlags;

            RenderTargets() : depthStencil(nullptr), temporaryFlags(0)
            {}

            const RenderTarget& getFirstTarget() const
            {
                return this->colors.empty() ? this->depthStencil : this->colors[0];
            }

            bool operator==(const RenderTargets& other) const
            {
                size_t numColors = this->colors.size();
                if (numColors != other.colors.size())
                    return false;

                for (size_t index = 0; index < numColors; index++)
                {
                    if (this->colors[index] != other.colors[index])
                        return false;
                }

                if (this->depthStencil != other.depthStencil || this->temporaryFlags != other.temporaryFlags)
                    return false;

                return true;
            }
        };

        struct DisplayState
        {
            DisplayState()
            {}

            Color color           = Color(1.0f, 1.0f, 1.0f, 1.0f);
            Color backgroundColor = Color(0.0f, 0.0f, 0.0f, 1.0f);

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

            StrongRef<FontBase> font;
            StrongRef<ShaderBase> shader;

            RenderTargetsStrongRef renderTargets;

            ColorChannelMask colorMask;

            bool useCustomProjection = false;
            Matrix4 customProjection;

            SamplerState defaultSamplerState = SamplerState();
        };

        GraphicsBase(const char* name);

        virtual ~GraphicsBase();

        void restoreState(const DisplayState& state);

        void restoreStateChecked(const DisplayState& state);

        bool isRenderTargetActive() const;

        bool isRenderTargetActive(TextureBase* texture) const;

        bool isRenderTargetActive(TextureBase* texture, int slice) const;

        void setActive(bool active)
        {
            this->flushBatchedDraws();
            this->active = active;
        }

        bool isActive() const;

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

        Color getBackgroundColor() const
        {
            return this->states.back().backgroundColor;
        }

        void setFont(FontBase* font)
        {
            this->states.back().font.set(font);
        }

        FontBase* getFont()
        {
            this->checkSetDefaultFont();
            return this->states.back().font.get();
        }

        const SamplerState& getDefaultSamplerState() const
        {
            return this->states.back().defaultSamplerState;
        }

        void setDefaultSamplerState(const SamplerState& state)
        {
            this->states.back().defaultSamplerState = state;
        }

        virtual void setScissor(const Rect& scissor) = 0;

        virtual void setScissor() = 0;

        void setShader();

        void setShader(ShaderBase* shader);

        void intersectScissor(const Rect& scissor);

        bool getScissor(Rect& scissor) const;

        void setMeshCullMode(CullMode mode)
        {
            this->states.back().meshCullMode = mode;
        }

        CullMode getMeshCullMode() const
        {
            return this->states.back().meshCullMode;
        }

        virtual void setFrontFaceWinding(Winding winding) = 0;

        Winding getFrontFaceWinding() const;

        virtual void setColorMask(ColorChannelMask mask) = 0;

        ColorChannelMask getColorMask() const;

        void setBlendMode(BlendMode mode, BlendAlpha alphaMode);

        Quad* newQuad(Quad::Viewport viewport, double sourceWidth, double sourceHeight) const;

        virtual TextureBase* newTexture(const TextureBase::Settings& settings,
                                        const TextureBase::Slices* data = nullptr) = 0;

        virtual FontBase* newFont(Rasterizer* data) = 0;

        virtual FontBase* newDefaultFont(int size, const Rasterizer::Settings& settings) = 0;

        // Mesh* newMesh(int vertexCount, PrimitiveType mode);

        TextBatch* newTextBatch(FontBase* font, const std::vector<ColoredString>& text = {});

        void checkSetDefaultFont();

        void print(const std::vector<ColoredString>& string, const Matrix4& matrix);

        void print(const std::vector<ColoredString>& string, FontBase* font, const Matrix4& matrix);

        void printf(const std::vector<ColoredString>& string, float wrap, FontBase::AlignMode align,
                    const Matrix4& matrix);

        void printf(const std::vector<ColoredString>& string, FontBase* font, float wrap,
                    FontBase::AlignMode align, const Matrix4& matrix);

        virtual ShaderStageBase* newShaderStageInternal(ShaderStageType stage,
                                                        const std::string& filepath) = 0;

        ShaderStageBase* newShaderStage(ShaderStageType stage, const std::string& filepath);

        virtual ShaderBase* newShaderInternal(StrongRef<ShaderStageBase> stages[SHADERSTAGE_MAX_ENUM],
                                              const ShaderBase::CompileOptions& options) = 0;

        ShaderBase* newShader(const std::vector<std::string>& filepaths,
                              const ShaderBase::CompileOptions& options);

        SpriteBatch* newSpriteBatch(TextureBase* texture, int size, BufferDataUsage usage);

        virtual void initCapabilities() = 0;

        TextureBase* getDefaultTexture(TextureBase* texture);

        TextureBase* getDefaultTexture(TextureType type, DataBaseType dataType);

        BlendMode getBlendMode(BlendAlpha& alphaMode) const
        {
            return computeBlendMode(this->states.back().blend, alphaMode);
        }

        virtual void setBlendState(const BlendState& blend) = 0;

        void captureScreenshot(const ScreenshotInfo& info);

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

        virtual void setPointSize(float size) = 0;

        float getPointSize() const
        {
            return this->states.back().pointSize;
        }

        Capabilities getCapabilities() const
        {
            return this->capabilities;
        }

        PixelFormat getSizedFormat(PixelFormat format);

        RendererInfo getRendererInfo() const;

        BatchedVertexData requestBatchedDraw(const BatchedDrawCommand& command);

        void flushBatchedDraws();

        static void flushBatchedDrawsGlobal();

        void advanceStreamBuffers();

        static void advanceStreamBuffersGlobal();

        virtual void draw(const DrawIndexedCommand& command) = 0;

        virtual void draw(const DrawCommand& command) = 0;

        Stats getStats() const;

        size_t getStackDepth() const
        {
            return this->stackTypeStack.size();
        }

        void push(StackType type = STACK_TRANSFORM);

        void pop();

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

        int getWidth() const;

        int getHeight() const;

        void applyTransform(const Matrix4& transform);

        void replaceTransform(const Matrix4& transform);

        Vector2 transformPoint(Vector2 point) const;

        Vector2 inverseTransformPoint(Vector2 point) const;

        void updateDeviceProjection(const Matrix4& projection)
        {
            this->deviceProjectionMatrix = projection;
        }

        void backbufferChanged(int width, int height, double pixelWidth, double pixelHeight);

        const Matrix4& getDeviceProjection() const
        {
            return this->deviceProjectionMatrix;
        }

        void resetProjection();

        void reset();

        virtual void clear(OptionalColor color, OptionalInt stencil, OptionalDouble depth) = 0;

        virtual void clear(const std::vector<OptionalColor>& colors, OptionalInt stencil,
                           OptionalDouble depth) = 0;

        virtual void present(void* screenshotCallbackData) = 0;

        virtual bool setMode(int width, int height, int pixelwidth, int pixelheight, bool backbufferstencil,
                             bool backbufferdepth, int msaa) = 0;

        virtual void unsetMode() = 0;

        virtual void setActiveScreen()
        {}

        virtual void setRenderTargetsInternal(const RenderTargets& targets, int pixelWidth, int pixelHeight,
                                              bool hasSRGBTexture) = 0;

        RenderTargets getRenderTargets() const;

        void setRenderTarget(RenderTarget target, uint32_t flags);

        void setRenderTargets(const RenderTargetsStrongRef& targets);

        void setRenderTargets(const RenderTargets& targets);

        void setRenderTarget();

        virtual bool isPixelFormatSupported(PixelFormat format, uint32_t usage) = 0;

        double getCurrentDPIScale() const;

        double getScreenDPIScale() const;

        void polyline(std::span<const Vector2> vertices);

        void polygon(DrawMode mode, std::span<const Vector2> vertices, bool skipLastFilledVertex = true);

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

        void draw(Drawable* drawable, const Matrix4& matrix);

        void draw(TextureBase* texture, Quad* quad, const Matrix4& matrix);

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
        TextureBase* defaultTextures[TEXTURE_MAX_ENUM];

      protected:
        int calculateEllipsePoints(float a, float b) const;

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
        int renderTargetSwitchCount;

        BatchedDrawState batchedDrawState;
        std::vector<uint8_t> scratchBuffer;

        float cpuProcessingTime;
        float gpuDrawingTime;

        Capabilities capabilities;

        StrongRef<FontBase> defaultFont;

        std::vector<ScreenshotInfo> pendingScreenshotCallbacks;
    };
} // namespace love
