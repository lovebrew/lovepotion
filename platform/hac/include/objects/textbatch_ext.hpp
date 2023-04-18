#pragma once

#include <objects/textbatch/textbatch.tcc>
#include <utilities/driver/vertex_ext.hpp>

#include <vector>

namespace love
{
    template<>
    class TextBatch<Console::HAC> : public TextBatch<Console::ALL>
    {
      public:
        TextBatch(Font<Console::HAC>* font, const Font<>::ColoredStrings& text = {});

        ~TextBatch();

        void Set(const Font<>::ColoredStrings& text);

        void Set(const Font<>::ColoredStrings& text, float wrap, Font<>::AlignMode align);

        int Add(const Font<>::ColoredStrings& text, const Matrix4<Console::HAC>& transform);

        int Addf(const Font<>::ColoredStrings& text, float wrap, Font<>::AlignMode mode,
                 const Matrix4<Console::HAC>& transform);

        void Clear();

        void SetFont(Font<Console::HAC>* font);

        int GetWidth(int index = 0) const;

        int GetHeight(int index = 0) const;

        virtual void Draw(Graphics<Console::HAC>& graphics,
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
            Matrix4<Console::HAC> matrix;
        };

        std::vector<Font<Console::HAC>::DrawCommand> drawCommands;
        std::vector<TextData> textData;
        size_t vertexOffset;
        std::vector<vertex::Vertex> vertexBuffer;
        uint32_t textureCacheId;

        void UploadVertices(const std::vector<vertex::Vertex>& vertices, size_t offset);

        void RegenerateVertices();

        void AddTextData(const TextData& data);
    };
} // namespace love
