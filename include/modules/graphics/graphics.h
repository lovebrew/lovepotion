/*
** modules/graphics.h
** @brief : handles graphical drawing
*/

#pragma once

#include "common/colors.h"
#include "common/lmath.h"
#include "common/screen.h"
#include "common/vector.h"

#include "common/render/batchdraw.h"
#include "common/render/renderstate.h"
#include "common/render/samplerstate.h"
#include "common/render/vertex.h"

#include "common/module.h"

/* OBJECTS */

#include "objects/texture/texture.h"

#include "objects/image/image.h"
#include "objects/image/wrap_image.h"

#include "objects/font/font.h"
#include "objects/font/wrap_font.h"

#include "objects/quad/quad.h"
#include "objects/quad/wrap_quad.h"

#include "objects/canvas/canvas.h"
#include "objects/canvas/wrap_canvas.h"

#include "objects/transform/transform.h"
#include "objects/transform/wrap_transform.h"

#include "objects/text/text.h"
#include "objects/text/wrap_text.h"

#include "modules/font/fontmodule.h"

#include "objects/imagedata/imagedata.h"

#include "objects/video/video.h"
#include "objects/videostream/videostream.h"

#include "objects/shader/shader.h"

#include <optional>
#include <vector>

namespace love
{
    class Renderer;

    class Graphics : public Module
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
            LINE_ROUGH,
            LINE_SMOOTH,
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

        static constexpr float MIN_DEPTH         = 1.0f / 16384.0f;
        static inline float CURRENT_DEPTH        = 0;
        static inline RenderScreen ACTIVE_SCREEN = 0;

        std::vector<Matrix4> transformStack;

        /* Gamma Correction */

        static bool gammaCorrectColor;

        static void SetGammaCorrect(bool enable);

        static bool IsGammaCorrect();

        static void GammaCorrectColor(Colorf& c);

        static void UnGammaCorrectColor(Colorf& c);

        static Colorf GammaCorrectColor(const Colorf& c);

        static Colorf UnGammaCorrectColor(const Colorf& c);

        /* End */

        Graphics();

        virtual ~Graphics();

        ModuleType GetModuleType() const
        {
            return M_GRAPHICS;
        }

        const char* GetName() const override
        {
            return "love.graphics";
        }

        template<typename T>
        static void CheckStandardTransform(lua_State* L, int idx, const T& func)
        {
            float x  = (float)luaL_optnumber(L, idx + 0, 0.0);
            float y  = (float)luaL_optnumber(L, idx + 1, 0.0);
            float a  = (float)luaL_optnumber(L, idx + 2, 0.0);
            float sx = (float)luaL_optnumber(L, idx + 3, 1.0);
            float sy = (float)luaL_optnumber(L, idx + 4, sx);
            float ox = (float)luaL_optnumber(L, idx + 5, 0.0);
            float oy = (float)luaL_optnumber(L, idx + 6, 0.0);
            float kx = (float)luaL_optnumber(L, idx + 7, 0.0);
            float ky = (float)luaL_optnumber(L, idx + 8, 0.0);

            func(Matrix4(x, y, a, sx, sy, ox, oy, kx, ky));
        }

        Matrix4 GetTransform()
        {
            return this->transformStack.back();
        }

        bool IsActive() const;

        bool IsCreated() const;

        Colorf GetColor() const;

        Colorf GetBackgroundColor() const;

        void SetBackgroundColor(const Colorf& color);

        /* render screen */

        void SetActiveScreen(RenderScreen screen);

        const RenderScreen GetActiveScreen() const;

        const int GetWidth(RenderScreen screen) const;

        const int GetHeight() const;

        std::vector<const char*> GetScreens() const;

        /* end screens */

        bool GetScissor(Rect& scissor) const;

        void IntersectScissor(const Rect& scissor);

        void Push(StackType type = STACK_TRANSFORM);

        void ApplyTransform(Transform* transform);

        void ReplaceTransform(Transform* transform);

        void Origin();

        void Translate(float offsetX, float offsetY);

        void Rotate(float rotation);

        void Scale(float scalarX, float ScalarY);

        void Shear(float kx, float ky);

        void Pop();

        /* Objects */

        Image* NewImage(const Image::Slices& data);

        Font* NewFont(Rasterizer* rasterizer);
#if defined(__SWITCH__)
        virtual Font* NewDefaultFont(int size, TrueTypeRasterizer::Hinting hinting) = 0;
#elif defined(__3DS__)
        virtual Font* NewDefaultFont(int size)                                   = 0;
#endif

        Image* NewImage(Texture::TextureType t, PixelFormat format, int width, int height,
                        int slices);

        Quad* NewQuad(Quad::Viewport v, double sw, double sh);

        Text* NewText(Font* font, const std::vector<Font::ColoredString>& text = {});

        void SetFont(Font* font);

        Font* GetFont();

        Video* NewVideo(VideoStream* stream, float dpiscale);

        float GetPointSize() const;

        LineStyle GetLineStyle() const;

        LineJoin GetLineJoin() const;

        float GetLineWidth() const;

        void SetLineJoin(LineJoin join);

        void SetLineStyle(LineStyle style);

        Canvas* NewCanvas(const Canvas::Settings& settings);

        Canvas* GetCanvas() const;

        void Draw(Drawable* drawable, const Matrix4& matrix);
        void Draw(Texture* texture, Quad* quad, const Matrix4& matrix);

        void Print(const std::vector<Font::ColoredString>& strings, const Matrix4& localTransform);
        void Print(const std::vector<Font::ColoredString>& strings, Font* font,
                   const Matrix4& localTransform);

        void PrintF(const std::vector<Font::ColoredString>& strings, float wrap,
                    Font::AlignMode align, const Matrix4& localTransform);
        void PrintF(const std::vector<Font::ColoredString>& strings, Font* font, float wrap,
                    Font::AlignMode align, const Matrix4& localTransform);

        virtual void SetDefaultSamplerState(const SamplerState& state);

        const SamplerState& GetDefaultSamplerState() const;

        /* virtual void stuff  -- subclass implements */

        virtual void SetScissor(const Rect& rect);

        virtual void SetScissor();

        void Clear(OptionalColor color, std::optional<int> stencil, std::optional<double> depth);

        void Clear(std::vector<OptionalColor>& colors, std::optional<int> stencil,
                   std::optional<double> depth);

        RenderState::BlendMode GetBlendMode(RenderState::BlendAlpha& alphaMode);

        const RenderState::BlendState& GetBlendState() const;

        void SetBlendState(const RenderState::BlendState& state);

        void SetColorMask(const RenderState::ColorMask& mask);

        virtual void SetColor(Colorf color);

        /* Primitives */

        virtual void Rectangle(DrawMode mode, float x, float y, float width, float height) = 0;

        virtual void Rectangle(DrawMode mode, float x, float y, float width, float height, float rx,
                               float ry) = 0;

        virtual void Rectangle(DrawMode mode, float x, float y, float width, float height, float rx,
                               float ry, int points) = 0;

        virtual void Ellipse(DrawMode mode, float x, float y, float a, float b) = 0;

        virtual void Ellipse(DrawMode mode, float x, float y, float a, float b, int points) = 0;

        virtual void Circle(DrawMode mode, float x, float y, float radius) = 0;

        virtual void Circle(DrawMode mode, float x, float y, float radius, int points) = 0;

#if defined(__SWITCH__)
        virtual void Polygon(DrawMode mode, const Vector2* points, size_t count,
                             bool skipLastFilledVertex = true) = 0;
#else
        virtual void Polygon(DrawMode mode, const Vector2* points, size_t count) = 0;
#endif

        virtual void Arc(DrawMode drawmode, ArcMode arcmode, float x, float y, float radius,
                         float angle1, float angle2) = 0;

        virtual void Arc(DrawMode drawmode, ArcMode arcmode, float x, float y, float radius,
                         float angle1, float angle2, int points) = 0;

        virtual void Points(const Vector2* points, size_t count, const Colorf* colors,
                            size_t colorCount) = 0;

        void SetPointSize(float size);

        virtual void Line(const Vector2* points, int count) = 0;

        void SetLineWidth(float width);

        Vector2 TransformPoint(Vector2 point);

        Vector2 InverseTransformPoint(Vector2 point);

        /* Primitives */

        RenderState::ColorMask GetColorMask() const;

        void SetMeshCullMode(Vertex::CullMode cull);

        void SetFrontFaceWinding(Vertex::Winding winding);

        Vertex::CullMode GetMeshCullMode() const;

        Vertex::Winding GetFrontFaceWinding() const;

        void SetShader();

        void SetShader(Shader* shader);

        Shader* GetShader() const;

        size_t GetStackDepth() const
        {
            return this->stackTypeStack.size();
        }

        /* RenderTarget Stuff */

        enum TemporaryRenderTargetFlags
        {
            TEMPORARY_RT_DEPTH   = (1 << 0),
            TEMPORARY_RT_STENCIL = (1 << 1),
        };

        struct RenderTargetStrongRef;

        struct RenderTarget
        {
            Canvas* canvas;
            int mipmap;

            RenderTarget(Canvas* canvas, int mipmap = 0) : canvas(canvas), mipmap(mipmap)
            {}

            RenderTarget() : canvas(nullptr), mipmap(0)
            {}

            bool operator!=(const RenderTarget& other) const
            {
                return canvas != other.canvas || mipmap != other.mipmap;
            }

            bool operator!=(const RenderTargetStrongRef& other) const
            {
                return canvas != other.canvas.Get() || mipmap != other.mipmap;
            }
        };

        struct RenderTargetStrongRef
        {
            StrongReference<Canvas> canvas;
            int mipmap = 0;

            RenderTargetStrongRef(Canvas* canvas, int mipmap = 0) : canvas(canvas), mipmap(mipmap)
            {}

            bool operator!=(const RenderTargetStrongRef& other) const
            {
                return canvas.Get() != other.canvas.Get() || mipmap != other.mipmap;
            }

            bool operator!=(const RenderTarget& other) const
            {
                return canvas.Get() != other.canvas || mipmap != other.mipmap;
            }
        };

        struct RenderTargets
        {
            std::vector<RenderTarget> colors;
            RenderTarget depthStencil;

            uint32_t temporaryRTFlags;

            RenderTargets() : depthStencil(nullptr), temporaryRTFlags(0)
            {}

            const RenderTarget& GetFirstTarget() const
            {
                return colors.empty() ? depthStencil : colors[0];
            }

            bool operator==(const RenderTargets& other) const
            {
                size_t ncolors = colors.size();
                if (ncolors != other.colors.size())
                    return false;

                for (size_t i = 0; i < ncolors; i++)
                {
                    if (colors[i] != other.colors[i])
                        return false;
                }

                if (depthStencil != other.depthStencil ||
                    temporaryRTFlags != other.temporaryRTFlags)
                    return false;

                return true;
            }
        };

        struct RenderTargetsStrongRef
        {
            std::vector<RenderTargetStrongRef> colors;
            RenderTargetStrongRef depthStencil;

            uint32_t temporaryRTFlags;

            RenderTargetsStrongRef() : depthStencil(nullptr), temporaryRTFlags(0)
            {}

            const RenderTargetStrongRef& GetFirstTarget() const
            {
                return colors.empty() ? depthStencil : colors[0];
            }
        };

        class TempTransform
        {
          public:
            TempTransform(Graphics* gfx) : gfx(gfx)
            {
                gfx->PushTransform();
            }

            TempTransform(Graphics* gfx, const Matrix4& t) : gfx(gfx)
            {
                gfx->PushTransform();
                gfx->transformStack.back() *= t;
            }

            template<typename vDst, typename vSrc>
            void TransformXY(vDst dst, vSrc src, int count)
            {
                gfx->GetTransform().TransformXY(dst, src, count);
            }

            ~TempTransform()
            {
                gfx->PopTransform();
            }

          private:
            Graphics* gfx;
        };

        struct Stats
        {
            int drawCalls;
            int drawCallsBatched;

            int canvasSwitches;
            int shaderSwitches;

            int canvases;
            int images;
            int fonts;

            int textureMemory;
        };

        Graphics::Stats GetStats() const;

        void PushTransform();

        void PopTransform();

        void SetCanvas(Canvas* canvas);

        bool IsCanvasActive(Canvas* canvas) const;

        bool IsCanvasActive() const;

        /* States or Something */
        void Reset();

        void Present();

        bool SetMode(int width, int height);

        virtual BatchedVertexData RequestBatchedDraw(const BatchedDrawCommand& command)
        {
            BatchedVertexData data {};

            return data;
        }

        virtual void FlushBatchedDraws()
        {}

        static void FlushBatchedDrawsGlobal();

        static Renderer& GetRenderer();

        static bool GetConstant(const char* in, DrawMode& out);
        static bool GetConstant(DrawMode in, const char*& out);
        static std::vector<const char*> GetConstants(DrawMode);

        static bool GetConstant(const char* in, ArcMode& out);
        static bool GetConstant(ArcMode in, const char*& out);
        static std::vector<const char*> GetConstants(ArcMode);

        static bool GetConstant(const char* in, StackType& out);
        static bool GetConstant(StackType in, const char*& out);
        static std::vector<const char*> GetConstants(StackType);

        static bool GetConstant(const char* in, LineStyle& out);
        static bool GetConstant(LineStyle in, const char*& out);
        static std::vector<const char*> GetConstants(LineStyle);

        static bool GetConstant(const char* in, LineJoin& out);
        static bool GetConstant(LineJoin in, const char*& out);
        static std::vector<const char*> GetConstants(LineJoin);

      protected:
        size_t drawCallsBatched = 0;

        struct DisplayState
        {
            DisplayState();

            Colorf foreground = Colorf(1, 1, 1, 1);
            Colorf background = Colorf(0, 0, 0, 1);

            RenderState::BlendState blend = RenderState::ComputeBlendState(
                RenderState::BLEND_ALPHA, RenderState::BLENDALPHA_MULTIPLY);

            float lineWidth     = 2.0f;
            LineStyle lineStyle = LINE_SMOOTH;
            LineJoin lineJoin   = LINE_JOIN_MITER;

            float pointSize = 1.0f;

            StrongReference<Font> font;
            StrongReference<Canvas> canvas;

            bool scissor     = false;
            Rect scissorRect = Rect();

            RenderState::CompareMode depthTest = RenderState::COMPARE_ALWAYS;
            bool depthWrite                    = false;

            Vertex::CullMode cullMode = Vertex::CULL_NONE;
            Vertex::Winding winding   = Vertex::WINDING_CCW;

            StrongReference<Shader> shader;

            RenderState::ColorMask colorMask = RenderState::ColorMask(true, true, true, true);
            float defaultMipmapSharpness     = 0.0f;

            SamplerState defaultSamplerState = SamplerState();
        };

        StreamDrawState streamDrawState;

        std::vector<DisplayState> states;
        std::vector<StackType> stackTypeStack;
        std::vector<double> pixelScaleStack;

        void RestoreState(const DisplayState& state);

        void RestoreStateChecked(const DisplayState& state);

      private:
        void CheckSetDefaultFont();

        bool active;
        bool created;

        StrongReference<Font> defaultFont;
    };
} // namespace love
