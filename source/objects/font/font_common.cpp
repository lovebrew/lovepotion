#include "common/runtime.h"
#include "objects/font/font.h"

using namespace love;

void Font::Print(const std::vector<Font::ColoredString> & strings, const DrawArgs & args, float * limit, const Color & blend)
{
    std::pair<float, float> offset = {0.0f, 0.0f};
    std::string line = "";

    for (size_t i = 0; i < strings.size(); i++)
    {
        const std::string & str = strings[i].string;
        auto currentChar = str.begin();
        const Color & clr = strings[i].color;

        Colors::DEBUG_COLOR(clr);

        while (currentChar != str.end())
        {
            if (*currentChar == '\n')
            {
                std::pair<float, float> size = this->GenerateVertices(line, offset, args, blend, clr);

                offset.first = 0.0f;
                offset.second += size.second;

                line = "";
            }
            else
                line += *currentChar;

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
