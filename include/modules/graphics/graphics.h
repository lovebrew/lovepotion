/*
** modules/graphics.h
** @brief : handles graphical drawing
*/

#pragma once

#include "objects/texture/texture.h"

#include "objects/image/wrap_image.h"
#include "objects/image/image.h"

#include "objects/font/wrap_font.h"
#include "objects/font/font.h"

#include "objects/quad/wrap_quad.h"
#include "objects/quad/quad.h"

#include "objects/canvas/wrap_canvas.h"
#include "objects/canvas/canvas.h"

#include "common/mmath.h"
#include "common/colors.h"

#if defined (_3DS)
    #define RENDERER_NAME "OpenGL ES"
    #define RENDERER_VERSION "1.1"
    #define RENDERER_VENDOR "Digital Media Professionals Inc."
    #define RENDERER_DEVICE "DMP PICA200"
#elif defined (__SWITCH__)
    #define RENDERER_NAME "OpenGL"
    #define RENDERER_VERSION "4.5"
    #define RENDERER_VENDOR "NVIDIA"
    #define RENDERER_DEVICE "Tegra X1"
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
                DRAW_FILL
            };

            struct DisplayState
            {
                Colorf foreground = { 1, 1, 1, 1 };
                Colorf background = { 0, 0, 0, 1 };

                float lineWidth = 1.0f;
                float pointSize = 1.0f;

                StrongReference<Font> font;

                Rect scissorRect;
                bool scissor = false;

                Texture::Filter defaultFilter = Texture::Filter();
                Texture::FilterMode defaultMipmapFilter = Texture::FILTER_LINEAR;
            };

            struct Point
            {
                float x;
                float y;
            };

            struct RendererInfo
            {
                std::string name    = RENDERER_NAME;
                std::string version = RENDERER_VERSION;
                std::string vendor  = RENDERER_VENDOR;
                std::string device  = RENDERER_DEVICE;
            };

            std::vector<Matrix4> transformStack;

            void Transform(DrawArgs * args, bool isTexture = false);
            void Transform(float * x, float * y);
            void TransformScale(float * x, float * y);

            static constexpr float MIN_DEPTH = 1.0f/16384.0f;
            static inline float CURRENT_DEPTH = 0;

            Graphics();
            ~Graphics();

            ModuleType GetModuleType() const { return M_GRAPHICS; }

            const char * GetName() const override { return "love.graphics"; }

            virtual void Clear(std::optional<Colorf> color, std::optional<int> stencil, std::optional<double> depth) = 0;

            void Clear(float r, float g, float b);

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

            Colorf GetColor();

            void SetColor(const Colorf & color);

            Colorf GetBackgroundColor();

            void SetBackgroundColor(const Colorf & color);

            float GetLineWidth();

            void SetLineWidth(float width);

            void GetDimensions(int * width, int * height);

            void SetScissor(const Rect & rect);

            void SetScissor();

            void SetDefaultFilter(const Texture::Filter & filter);

            const Texture::Filter & GetDefaultFilter() const;

            Rect GetScissor();

            void Push();

            void Origin();

            void Translate(float offsetX, float offsetY);

            void SetDepth(float depth);

            void Rotate(float rotation);

            void Scale(float scalarX, float ScalarY);

            void Shear(float kx, float ky);

            void Pop();

            RendererInfo GetRendererInfo();

            /* Objects */

            Image * NewImage(Data * data);

            Font * NewFont(Font::SystemFontType type, float size = Font::DEFAULT_SIZE);

            Font * NewFont(Data * data, float size = Font::DEFAULT_SIZE);

            Font * NewFont(float size = Font::DEFAULT_SIZE);

            Quad * NewQuad(Quad::Viewport v, double sw, double sh);

            void SetFont(Font * font);

            Font * GetFont();

            Canvas * NewCanvas(const Canvas::Settings & settings);

            void Draw(Drawable * drawable, const Matrix4 & matrix);
            void Draw(Texture * texture, Quad * quad, const Matrix4 & matrix);

            void Print(const std::vector<Font::ColoredString> & strings, const Matrix4 & localTransform);
            void Print(const  std::vector<Font::ColoredString> & strings, Font * font, const Matrix4 & localTransform);

            void PrintF(const std::vector<Font::ColoredString> & strings, float wrap, Font::AlignMode align, const Matrix4 & localTransform);
            void PrintF(const  std::vector<Font::ColoredString> & strings, Font * font, float wrap, Font::AlignMode align, const Matrix4 & localTransform);

            void SetCanvas(Canvas * canvas);

            /* Graphics Primitives */

            void Circle(float x, float y, float radius);

            void Rectangle(const std::string & mode, float x, float y, float width, float height, float rx, float ry);

            void Polygon(const std::string & mode, std::vector<Graphics::Point> points);

            void Line(float startx, float starty, float endx, float endy);

            void Arc(const std::string & mode, float x, float y, float radius, float startAngle, float endAngle);

            void Ellipse(const std::string & mode, float x, float y, float radiusX, float radiusY);

            static void SetViewMatrix(const Matrix4 & matrix);

            /* States or Something */

            void Reset();

            virtual void Present() = 0;

            std::vector<DisplayState> states;

            static inline bool GetConstant(const std::string & in, DrawMode & out) {
                if (m_modes.find(in) != m_modes.end())
                {
                    out = m_modes[in];
                    return true;
                }

                return false;
            }

        private:
            void RestoreState(const DisplayState & state);
            void CheckSetDefaultFont();

            void SetMode();

            StrongReference<Font> defaultFont;
            RendererInfo rendererInfo;

            static inline std::map<std::string, Graphics::DrawMode> m_modes =
            {
                { "line", DrawMode::DRAW_LINE },
                { "fill", DrawMode::DRAW_FILL }
            };

            float stereoDepth = 0.0f;
            bool isPushed = false;
    };
}
