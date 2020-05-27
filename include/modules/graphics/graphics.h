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

#include "common/mmath.h"

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
                Color foreground = { 1, 1, 1, 1 };
                Color background = { 0, 0, 0, 1 };

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

            struct TransformState
            {
                float offsetX = 0.0f;
                float offsetY = 0.0f;

                float rotation = 0.0f;

                float scalarX = 1.0f;
                float scalarY = 1.0f;
            };

            std::vector<TransformState> transformStack;
            void Transform(DrawArgs & args);

            static constexpr float MIN_DEPTH = 1.0f/16384.0f;
            static inline float CURRENT_DEPTH = 0;

            Graphics();
            ~Graphics();

            ModuleType GetModuleType() const { return M_GRAPHICS; }

            const char * GetName() const override { return "love.graphics"; }

            void Clear(float r, float g, float b);

            Color GetColor();

            void SetColor(const Color & color);

            Color GetBackgroundColor();

            void SetBackgroundColor(const Color & color);

            float GetLineWidth();

            void SetLineWidth(float width);

            void GetDimensions(int * width, int * height);

            void SetScissor(int x, int y, int width, int height);

            void SetDefaultFilter(const Texture::Filter & filter);

            const Texture::Filter & GetDefaultFilter() const;

            Rect GetScissor();

            void Push();

            void Translate(float offsetX, float offsetY);

            void SetDepth(float depth);

            void Rotate(float rotation);

            void Scale(float scalarX, float ScalarY);

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

            void Draw(Drawable * drawable, const DrawArgs & args);
            void Draw(Texture * texture, Quad * quad, const DrawArgs & args);

            void Print(const std::vector<Font::ColoredString> & strings, const DrawArgs & args);
            void Print(const  std::vector<Font::ColoredString> & strings, Font * font, const DrawArgs & args);

            void PrintF(const std::vector<Font::ColoredString> & strings, const DrawArgs & args, float wrap, Font::AlignMode align);
            void PrintF(const  std::vector<Font::ColoredString> & strings, Font * font, const DrawArgs & args, float wrap, Font::AlignMode align);

            /* Graphics Primitives */

            void Circle(float x, float y, float radius);

            void Rectangle(const std::string & mode, float x, float y, float width, float height);

            void Polygon(const std::string & mode, std::vector<Graphics::Point> points);

            void Line(float startx, float starty, float endx, float endy);

            /* States or Something */

            void Reset();

            void Present();

            Color AdjustColor(const Color & in);

            void AdjustColor(Color * in);

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
