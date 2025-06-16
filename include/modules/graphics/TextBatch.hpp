#pragma once

#include "common/Range.hpp"

#include "modules/graphics/Drawable.hpp"
#include "modules/graphics/Font.tcc"

namespace love
{
    class TextBatch : public Drawable
    {
      public:
        static Type type;

        TextBatch(FontBase* font, const std::vector<ColoredString>& text = {});

        virtual ~TextBatch();

        void set(const std::vector<ColoredString>& text);

        void set(const std::vector<ColoredString>& text, float wrap, FontBase::AlignMode align);

        int add(const std::vector<ColoredString>& text, const Matrix4& matrix);

        int addf(const std::vector<ColoredString>& text, float wrap, FontBase::AlignMode align,
                 const Matrix4& matrix);

        void clear();

        void setFont(FontBase* font);

        FontBase* getFont() const;

        int getWidth(int index = 0) const;

        int getHeight(int index = 0) const;

        void draw(GraphicsBase* graphics, const Matrix4& matrix) override;

      private:
        struct TextData
        {
            ColoredCodepoints codepoints;
            float wrap;
            FontBase::AlignMode align;
            TextShaper::TextInfo textInfo;
            bool useMatrix;
            bool appendVertices;
            Matrix4 matrix;
        };

        void uploadVertices(const std::vector<FontBase::GlyphVertex>& vertices, size_t offset);

        void regenerateVertices();

        void addTextData(const TextData& data);

        StrongRef<FontBase> font;

        VertexAttributes vertexAttributes;

        std::vector<Vertex> buffer;
        Range modifiedVertices;

        std::vector<FontBase::DrawCommand> drawCommands;
        std::vector<TextData> textData;

        size_t vertexOffset;
        uint32_t textureCacheID;
    };
} // namespace love
