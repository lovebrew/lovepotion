#pragma once

#include <common/console.hpp>
#include <common/drawable.hpp>
#include <common/exception.hpp>
#include <common/range.hpp>
#include <common/strongreference.hpp>

#include <objects/font/font.hpp>

namespace love
{
    class TextBatch : public Drawable
    {
      public:
        static inline Type type = Type("TextBatch", &Drawable::type);

        TextBatch(Font* font, const ColoredStrings& text = {});

        virtual ~TextBatch();

        void Set(const ColoredStrings& text);

        void Set(const ColoredStrings& text, float wrap, Font::AlignMode align);

        int Add(const ColoredStrings& text, const Matrix4& matrix);

        int Addf(const ColoredStrings& text, float wrap, Font::AlignMode align,
                 const Matrix4& matrix);

        void Clear();

        void SetFont(Font* font);

        Font* GetFont() const
        {
            return this->font.Get();
        }

        int GetWidth(int index = 0) const;

        int GetHeight(int index = 0) const;

        void Draw(Graphics<Console::Which>& graphics,
                  const Matrix4& matrix) override;

      private:
        struct TextData
        {
            ColoredCodepoints codepoints;
            float wrap;
            Font::AlignMode align;
            TextShaper::TextInfo textInfo;
            bool useMatrix;
            bool appendVertices;
            Matrix4 matrix;
        };

        void UploadVertices(const std::vector<vertex::Vertex>& vertices, size_t offset);

        void RegenerateVertices();

        void AddTextData(const TextData& data);

        StrongReference<Font> font;
        std::vector<vertex::Vertex> buffer;
        Range modifiedVertices;

        std::vector<Font::DrawCommand> drawCommands;
        std::vector<TextData> textData;
        size_t vertexOffset;

        uint32_t textureCacheId;
    };
} // namespace love
