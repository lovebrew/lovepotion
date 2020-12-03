#pragma once

#include "objects/text/textc.h"

namespace love
{
    class Text : public common::Text
    {
        public:
            Text(Font * font, const std::vector<Font::ColoredString> & text = {});

            void Set(const std::vector<Font::ColoredString> & text) override;

            void Set(const std::vector<Font::ColoredString> & text, float wrap, Font::AlignMode align) override;

        private:
            struct TextData
            {
                float wrap;
                Font::AlignMode align;
                bool useMatrix;
                Matrix4 matrix;
            };

            std::vector<TextData> textData;
    };
}