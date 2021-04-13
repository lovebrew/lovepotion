#pragma once

#include "deko3d/vertex.h"
#include "objects/text/textc.h"

namespace love
{
    class Text : public common::Text
    {
      public:
        Text(Font* font, const std::vector<Font::ColoredString>& text = {});

        virtual ~Text();

        void SetFont(Font* font);

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

      private:
        struct TextData
        {
            Font::ColoredCodepoints codepoints;
            float wrap;
            Font::AlignMode align;
            Font::TextInfo textInfo;
            bool useMatrix;
            bool appendVertices;
            Matrix4 matrix;
        };

        std::vector<Font::DrawCommand> drawCommands;

        std::vector<TextData> textData;

        std::vector<vertex::GlyphVertex> vertexBuffer;

        size_t vertexOffset;

        uint32_t textureCacheId;

        void CopyVertices(const std::vector<vertex::GlyphVertex>& vertices, size_t vertoffset);

        void RegenerateVertices();

        void AddTextData(const TextData& textData);
    };
} // namespace love
