/*
** modules/graphics.h
** @brief : handles graphical drawing
*/

#pragma once

#include "objects/texture/texture.h"

#include "objects/image/wrap_image.h"
#include "objects/image/image.h"

#include "objects/font/font.h"
#include "objects/font/wrap_font.h"

#include "objects/quad/wrap_quad.h"
#include "objects/quad/quad.h"

#include "objects/canvas/wrap_canvas.h"
#include "objects/canvas/canvas.h"

#include "objects/transform/wrap_transform.h"
#include "objects/transform/transform.h"

#include "common/mmath.h"
#include "common/colors.h"

#if defined(__SWITCH__)
    // #include "objects/font.h"
    // using gFont = love::deko3d::Font;

    #include "deko3d/shader.h"
    #include "deko3d/vertex.h"
#endif

namespace love
{
    class Graphics : public Module
    {
        public:
            static const size_t MAX_USER_STACK_DEPTH = 128;

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

            enum BlendMode
            {
                BLEND_ALPHA,
                BLEND_ADD,
                BLEND_SUBTRACT,
                BLEND_MULTIPLY,
                BLEND_LIGHTEN,
                BLEND_DARKEN,
                BLEND_SCREEN,
                BLEND_REPLACE,
                BLEND_NONE,
                BLEND_MAX_ENUM
            };

            enum BlendAlpha
            {
                BLENDALPHA_MULTIPLY,
                BLENDALPHA_PREMULTIPLIED,
                BLENDALPHA_MAX_ENUM
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

            struct RendererInfo
            {
                std::string name;
                std::string version;
                std::string vendor;
                std::string device;
            };

            enum StackType
            {
                STACK_ALL,
                STACK_TRANSFORM,
                STACK_MAX_ENUM
            };

            struct ColorMask
            {
                bool r, g, b, a;

                ColorMask()
                    : r(true), g(true), b(true), a(true)
                {}

                ColorMask(bool _r, bool _g, bool _b, bool _a)
                    : r(_r), g(_g), b(_b), a(_a)
                {}

                bool operator == (const ColorMask & m) const
                {
                    return r == m.r && g == m.g && b == m.b && a == m.a;
                }

                bool operator != (const ColorMask & m) const
                {
                    return !(operator == (m));
                }
            };

            std::vector<Matrix4> transformStack;

            /* Gamma Correction */

            static bool gammaCorrectColor;

            static void SetGammaCorrect(bool enable);

            static bool IsGammaCorrect();

            static void GammaCorrectColor(Colorf & c);

            static void UnGammaCorrectColor(Colorf & c);

            static Colorf GammaCorrectColor(const Colorf & c);

            static Colorf UnGammaCorrectColor(const Colorf & c);

            /* End */

            static constexpr float MIN_DEPTH = 1.0f/16384.0f;
            static inline float CURRENT_DEPTH = 0;

            Graphics();
            ~Graphics();

            ModuleType GetModuleType() const { return M_GRAPHICS; }

            const char * GetName() const override { return "love.graphics"; }

            template <typename T>
            static void CheckStandardTransform(lua_State * L, int idx, const T & func)
            {
                float x  = (float) luaL_optnumber(L, idx + 0, 0.0);
                float y  = (float) luaL_optnumber(L, idx + 1, 0.0);
                float a  = (float) luaL_optnumber(L, idx + 2, 0.0);
                float sx = (float) luaL_optnumber(L, idx + 3, 1.0);
                float sy = (float) luaL_optnumber(L, idx + 4, sx);
                float ox = (float) luaL_optnumber(L, idx + 5, 0.0);
                float oy = (float) luaL_optnumber(L, idx + 6, 0.0);
                float kx = (float) luaL_optnumber(L, idx + 7, 0.0);
                float ky = (float) luaL_optnumber(L, idx + 8, 0.0);

                func(Matrix4(x, y, a, sx, sy, ox, oy, kx, ky));
            }

            Matrix4 GetTransform() {
                return this->transformStack.back();
            }

            Colorf GetColor() const;

            Colorf GetBackgroundColor() const;

            void SetBackgroundColor(const Colorf & color);

            void GetDimensions(int * width, int * height);

            bool GetScissor(Rect & scissor) const;

            void IntersectScissor(const Rect & scissor);

            const Texture::Filter & GetDefaultFilter() const;

            void Push(StackType type = STACK_TRANSFORM);

            void ApplyTransform(Transform * transform);

            void ReplaceTransform(Transform * transform);

            void Origin();

            void Translate(float offsetX, float offsetY);

            void SetDepth(float depth);

            void Rotate(float rotation);

            void Scale(float scalarX, float ScalarY);

            void Shear(float kx, float ky);

            void Pop();

            /* Objects */

            Image * NewImage(Data * data);

            #if defined (__SWITCH__)
                virtual Font * NewDefaultFont(int size, TrueTypeRasterizer::Hinting hinting, const Texture::Filter & filter = Texture::defaultFilter) = 0;

                virtual Font * NewFont(Rasterizer * rasterizer, const Texture::Filter & filter = Texture::defaultFilter) = 0;
            #elif defined (_3DS)
                virtual Font * NewDefaultFont(int size, const Texture::Filter & filter = Texture::defaultFilter) = 0;

                virtual Font * NewFont(const Rasterizer & rasterizer, const Texture::Filter & filter = Texture::defaultFilter) = 0;
            #endif


            Quad * NewQuad(Quad::Viewport v, double sw, double sh);

            void SetFont(Font * font);

            Font * GetFont();

            float GetPointSize() const;

            LineStyle GetLineStyle() const;

            LineJoin GetLineJoin() const;

            float GetLineWidth() const;

            void SetLineJoin(LineJoin join);

            void SetLineStyle(LineStyle style);

            Canvas * NewCanvas(const Canvas::Settings & settings);

            void Draw(Drawable * drawable, const Matrix4 & matrix);
            void Draw(Texture * texture, Quad * quad, const Matrix4 & matrix);

            void Print(const std::vector<Font::ColoredString> & strings, const Matrix4 & localTransform);
            void Print(const  std::vector<Font::ColoredString> & strings, Font * font, const Matrix4 & localTransform);

            void PrintF(const std::vector<Font::ColoredString> & strings, float wrap, Font::AlignMode align, const Matrix4 & localTransform);
            void PrintF(const  std::vector<Font::ColoredString> & strings, Font * font, float wrap, Font::AlignMode align, const Matrix4 & localTransform);

            void SetCanvas(Canvas * canvas);

            virtual void SetDefaultFilter(const Texture::Filter & filter);

            /* virtual void stuff  -- subclass implements */

            virtual void SetScissor(const Rect & rect) = 0;

            virtual void SetScissor() = 0;

            virtual void Clear(std::optional<Colorf> color, std::optional<int> stencil, std::optional<double> depth) = 0;

            virtual void SetBlendMode(BlendMode mode, BlendAlpha alpha) = 0;

            virtual void SetColorMask(ColorMask mask) = 0;

            virtual void SetColor(Colorf color);

            void SetDefaultMipmapFilter(Texture::FilterMode filter, float sharpness);

            #if defined (__SWITCH__)
                virtual void SetMeshCullMode(vertex::CullMode cull) = 0;

                virtual void SetFrontFaceWinding(vertex::Winding winding) = 0;
            #endif

            /* Primitives */

            virtual void Rectangle(DrawMode mode, float x, float y, float width, float height) = 0;

            virtual void Rectangle(DrawMode mode, float x, float y, float width, float height, float rx, float ry) = 0;

            virtual void Rectangle(DrawMode mode, float x, float y, float width, float height, float rx, float ry, int points) = 0;

            virtual void Ellipse(DrawMode mode, float x, float y, float a, float b) = 0;

            virtual void Ellipse(DrawMode mode, float x, float y, float a, float b, int points) = 0;

            virtual void Circle(DrawMode mode, float x, float y, float radius) = 0;

            virtual void Circle(DrawMode mode, float x, float y, float radius, int points) = 0;

            #if defined(__SWITCH__)
                virtual void Polygon(DrawMode mode, const Vector2 * points, size_t count, bool skipLastFilledVertex = true) = 0;
            #else
                virtual void Polygon(DrawMode mode, const Vector2 * points, size_t count) = 0;
            #endif

            virtual void Arc(DrawMode drawmode, ArcMode arcmode, float x, float y, float radius, float angle1, float angle2) = 0;

            virtual void Arc(DrawMode drawmode, ArcMode arcmode, float x, float y, float radius, float angle1, float angle2, int points) = 0;

            virtual void Points(const Vector2 * points, size_t count, const Colorf * colors, size_t colorCount) = 0;

            virtual void SetPointSize(float size) = 0;

            virtual void Line(float startx, float starty, float endx, float endy) = 0;

            virtual void SetLineWidth(float width) = 0;

            virtual RendererInfo GetRendererInfo() const = 0;

            Vector2 TransformPoint(Vector2 point);

            Vector2 InverseTransformPoint(Vector2 point);

            /* Primitives */

            ColorMask getColorMask() const;


            static void SetViewMatrix(const Matrix4 & matrix);

            #if defined(__SWITCH__)
                vertex::CullMode GetMeshCullMode() const;

                vertex::Winding GetFrontFaceWinding() const;

                void SetShader();

                void SetShader(Shader * shader);

                Shader * GetShader() const;
            #endif

            size_t GetStackDepth() const {
                return this->stackTypeStack.size();
            }

            /* States or Something */

            void Reset();

            virtual void Present() = 0;

            static bool GetConstant(const char * in, DrawMode & out);
            static bool GetConstant(DrawMode in, const char *& out);
            static std::vector<std::string> GetConstants(DrawMode);

            static bool GetConstant(const char * in, BlendMode & out);
            static bool GetConstant(BlendMode in, const char *& out);
            static std::vector<std::string> GetConstants(BlendMode);

            static bool GetConstant(const char * in, BlendAlpha & out);
            static bool GetConstant(BlendAlpha in, const char *& out);
            static std::vector<std::string> GetConstants(BlendAlpha);

            static bool GetConstant(const char * in, ArcMode & out);
            static bool GetConstant(ArcMode in, const char *& out);
            static std::vector<std::string> GetConstants(ArcMode);

            static bool GetConstant(const char *in, StackType & out);
            static bool GetConstant(StackType in, const char *& out);
            static std::vector<std::string> GetConstants(StackType);

        protected:
            struct DisplayState
            {
                Colorf foreground = Colorf(1, 1, 1, 1);
                Colorf background = Colorf(0, 0, 0, 1);

                float lineWidth = 1.0f;
                float pointSize = 1.0f;

                StrongReference<Font> font;

                #if defined(__SWITCH__)
                    StrongReference<Shader> shader;
                #endif

                Rect scissorRect = Rect();
                bool scissor = false;

                Texture::Filter defaultFilter = Texture::Filter();
                Texture::FilterMode defaultMipmapFilter = Texture::FILTER_LINEAR;
                float defaultMipmapSharpness = 0.0f;

                LineStyle lineStyle = LINE_SMOOTH;
                LineJoin lineJoin = LINE_JOIN_MITER;

                #if defined(__SWITCH__)
                    vertex::CullMode meshCullMode = vertex::CULL_NONE;
                    vertex::Winding winding = vertex::WINDING_CCW;
                #endif

                BlendMode blendMode = BLEND_ALPHA;
                BlendAlpha blendAlphaMode = BLENDALPHA_MULTIPLY;

                ColorMask colorMask = ColorMask(true, true, true, true);
            };

            std::vector<DisplayState> states;
            std::vector<StackType> stackTypeStack;
            std::vector<double> pixelScaleStack;

            void RestoreState(const DisplayState & state);

            void RestoreStateChecked(const DisplayState & state);

        private:
            void CheckSetDefaultFont();

            void SetMode();

            StrongReference<Font> defaultFont;
            RendererInfo rendererInfo;

            static StringMap<DrawMode, DRAW_MAX_ENUM>::Entry drawModeEntries[];
            static StringMap<DrawMode, DRAW_MAX_ENUM> drawModes;

            static StringMap<BlendMode, BLEND_MAX_ENUM>::Entry blendModeEntries[];
            static StringMap<BlendMode, BLEND_MAX_ENUM> blendModes;

            static StringMap<BlendAlpha, BLENDALPHA_MAX_ENUM>::Entry blendAlphaEntries[];
            static StringMap<BlendAlpha, BLENDALPHA_MAX_ENUM> blendAlphaModes;

            static StringMap<ArcMode, ARC_MAX_ENUM>::Entry arcModeEntries[];
            static StringMap<ArcMode, ARC_MAX_ENUM> arcModes;

            static StringMap<StackType, STACK_MAX_ENUM>::Entry stackTypeEntries[];
            static StringMap<StackType, STACK_MAX_ENUM> stackTypes;

            float stereoDepth = 0.0f;
            bool isPushed = false;
    };
}
