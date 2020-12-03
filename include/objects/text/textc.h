#pragma once

#include "objects/drawable/drawable.h"
#include "objects/font/font.h"

namespace love
{
    class Graphics;

    namespace common
    {
        class Text : public Drawable
        {
            public:
                static love::Type type;

                Text(Font * font, const std::vector<Font::ColoredString> & text = {});

                ~Text() = default;

                virtual void Set(const std::vector<Font::ColoredString> & text) = 0;

                virtual void Set(const std::vector<Font::ColoredString> & text, float wrap, Font::AlignMode align) = 0;

                virtual int Add(const std::vector<Font::ColoredString> & text, const Matrix4 & localTransform) = 0;

                virtual int Addf(const std::vector<Font::ColoredString> & text, float wrap, Font::AlignMode align, const Matrix4 & localTransform) = 0;

                virtual void Clear() = 0;

                virtual void SetFont(Font * font) = 0;

                Font * GetFont() const;

                virtual int GetWidth(int index = 0) const = 0;

                virtual int GetHeight(int index = 0) const = 0;

                void Draw(Graphics * gfx, const Matrix4 & m) override;

            protected:
                StrongReference<Font> font;
        };
    }
}