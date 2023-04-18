#pragma once

#include <objects/textbatch/textbatch.tcc>
#include <utilities/driver/vertex_ext.hpp>

#include <vector>

namespace love
{
    template<>
    class TextBatch<Console::CAFE> : public TextBatch<Console::ALL>
    {
      public:
        TextBatch(Font<Console::CAFE>* font, const Font<>::ColoredStrings& text = {});

        ~TextBatch();

        void Set(const Font<>::ColoredStrings& text);

        void Set(const Font<>::ColoredStrings& text, float wrap, Font<>::AlignMode align);

        int Add(const Font<>::ColoredStrings& text, const Matrix4<Console::CAFE>& transform);

        int Addf(const Font<>::ColoredStrings& text, float wrap, Font<>::AlignMode mode,
                 const Matrix4<Console::CAFE>& transform);

        void SetFont(Font<Console::CAFE>* font);

        void Clear();

        int GetWidth(int index = 0) const;

        int GetHeight(int index = 0) const;

        virtual void Draw(Graphics<Console::CAFE>& graphics,
                          const Matrix4<Console::Which>& matrix) override;

      private:
        struct TextData
        {
            Font<>::ColoredCodepoints codepoints;
            float wrap;
            Font<>::AlignMode align;
            Font<>::TextInfo textInfo;
            bool useMatrix;
            bool appendVertices;
            Matrix4<Console::CAFE> matrix;
        };

        std::vector<Font<Console::CAFE>::DrawCommand> drawCommands;
        std::vector<TextData> textData;
        size_t vertexOffset;
        std::vector<vertex::Vertex> vertexBuffer;
        uint32_t textureCacheId;

        void UploadVertices(const std::vector<vertex::Vertex>& vertices, size_t offset);

        void RegenerateVertices();

        void AddTextData(const TextData& data);
    };
} // namespace love
