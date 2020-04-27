#include "common/runtime.h"
#include "objects/font/font.h"

using namespace love;

void Font::Print(const std::vector<Font::ColoredString> & strings, const DrawArgs & args, float * limit, const Color & blend, Font::AlignMode align)
{
    std::pair<float, float> offset = {0.0f, 0.0f};
    std::string line = "";
    float currentSize = 0.0f;

    for (size_t i = 0; i < strings.size(); i++)
    {
        const std::string & str = strings[i].string;
        auto currentChar = str.begin();
        const Color & clr = strings[i].color;

        while (currentChar != str.end())
        {
            uint32_t codepoint;
            decode_utf8(&codepoint, (uint8_t *)&currentChar);
            currentSize += this->_GetGlyphWidth(codepoint);

            if (*currentChar == '\n' || (limit != nullptr && currentSize >= *limit))
            {
                std::pair<float, float> size = this->GenerateVertices(line, offset, args, blend, clr);

                offset.first = 0.0f;
                offset.second += size.second;
            }

            if (*currentChar == '\n')
                line = "";
            else
            {
                if (limit != nullptr && currentSize >= *limit)
                {
                    currentSize = 0.0f;
                    line = "";
                }

                line += *currentChar;
            }

            ++currentChar;
        }

        if (!line.empty())
        {
            std::pair<float, float> size = this->GenerateVertices(line, offset, args, blend, clr);
            offset.first += size.first;

            line = "";
        }
    }
}
