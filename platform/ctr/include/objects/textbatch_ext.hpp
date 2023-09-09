#pragma once

#include <objects/textbatch/textbatch.tcc>

#include <modules/graphics/graphics.tcc>

#include <numeric>

namespace love
{
    template<>
    class TextBatch<Console::CTR> : public TextBatch<Console::ALL>
    {
      public:
        struct AlignData
        {
            float limit;
            Font::AlignMode mode;
        };

        TextBatch(Font* font, const Font::ColoredStrings& text = {});

        virtual ~TextBatch();

        void AddText(const Font::ColoredStrings& text);

        void AddText(const Font::ColoredStrings& text, const Matrix4<Console::CTR>& matrix);

        void Set(const Font::ColoredStrings& text);

        void Set(const Font::ColoredStrings& text, float wrap, Font::AlignMode align);

        int Add(const Font::ColoredStrings& text, const Matrix4<Console::CTR>& transform);

        int Addf(const Font::ColoredStrings& text, float wrap, Font::AlignMode mode,
                 const Matrix4<Console::CTR>& transform);

        void Clear();

        int GetWidth(int index = 0) const;

        int GetHeight(int index = 0) const;

        virtual void Draw(Graphics<Console::CTR>& graphics,
                          const Matrix4<Console::Which>& matrix) override;

      private:
        std::vector<AlignData> wrapping;

        float scale;

        C2D_TextBuf buffer;
        C2D_Text text;

        std::vector<std::string> cache;
        std::string vertices;

        float wrap;
        Font::AlignMode align;
        std::vector<Font::TextInfo> textInfo;
    };
} // namespace love
