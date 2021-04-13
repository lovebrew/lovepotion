#pragma once

#include "objects/text/textc.h"

namespace love
{
    class Text : public common::Text
    {
      public:
        Text(love::Font* font, const std::vector<Font::ColoredString>& text = {});

        virtual ~Text();

        void SetFont(love::Font* font);

        void Set(const std::vector<Font::ColoredString>& text) override;

        void Set(const std::vector<Font::ColoredString>& text, float wrap,
                 Font::AlignMode align) override;

        int Add(const std::vector<Font::ColoredString>& text,
                const Matrix4& localTransform) override;

        int Addf(const std::vector<Font::ColoredString>& text, float wrap, Font::AlignMode align,
                 const Matrix4& localTransform) override;

        int GetWidth(int index = 0) const override;

        int GetHeight(int index = 0) const override;

        void Draw(Graphics* gfx, const Matrix4& localTransform) override;

        void Clear() override;

        std::string GetString(const std::vector<Font::ColoredString>& text);

      private:
        std::vector<std::pair<float, Font::AlignMode>> wrapData;

        C2D_TextBuf buffer;
        C2D_Text text;

        std::string textCache;

        float wrap;
        Font::AlignMode align;
    };
} // namespace love