#include "common/runtime.h"
#include "objects/font/font.h"

using namespace love;

void Font::Print(const std::vector<Font::ColoredString> & strings, const DrawArgs & args, float * limit, const Color & blend, Font::AlignMode align)
{
    std::pair<float, float> offset = {0.0f, 0.0f};
    std::string line;
    float currentSize = 0.0f;

    for (size_t i = 0; i < strings.size(); i++)
    {
        const std::string & str = strings[i].string;
        auto currentChar = str.c_str();
        const auto end = currentChar + str.size();
        const Color & clr = strings[i].color;

        while (currentChar != end)
        {
            uint32_t codepoint;
            const auto bytes = decode_utf8(&codepoint, (uint8_t *)currentChar);
            currentSize += this->_GetGlyphWidth(codepoint);

            if (codepoint == '\n' || (limit != nullptr && currentSize >= *limit))
            {
                std::pair<float, float> size = this->GenerateVertices(line, offset, args, blend, clr);

                offset.first = 0.0f;
                offset.second += size.second;
            }

            if (codepoint == '\n')
                line.clear();
            else
            {
                if (limit != nullptr && currentSize >= *limit)
                {
                    currentSize = 0.0f;
                    line.clear();
                }

                line.append(currentChar, bytes);
            }

            currentChar += bytes;
        }

        if (!line.empty())
        {
            std::pair<float, float> size = this->GenerateVertices(line, offset, args, blend, clr);
            offset.first += size.first;

            line.clear();
        }
    }
}

bool Font::GetConstant(const char * in, AlignMode & out)
{
    return alignModes.Find(in, out);
}

bool Font::GetConstant(AlignMode in, const char  *& out)
{
    return alignModes.Find(in, out);
}

std::vector<std::string> Font::GetConstants(AlignMode)
{
    return alignModes.GetNames();
}

StringMap<Font::AlignMode, Font::ALIGN_MAX_ENUM>::Entry Font::alignModeEntries[] =
{
    { "left",    ALIGN_LEFT    },
    { "right",   ALIGN_RIGHT   },
    { "center",  ALIGN_CENTER  },
    { "justify", ALIGN_JUSTIFY },
};

StringMap<Font::AlignMode, Font::ALIGN_MAX_ENUM> Font::alignModes(Font::alignModeEntries, sizeof(Font::alignModeEntries));
