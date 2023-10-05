#include <common/exception.hpp>

#include <objects/rasterizer/rasterizer.hpp>

#include <utilities/shaper/textshaper.hpp>

#include <utf8/utf8.h>

using namespace love;

void love::GetCodepointsFromString(const std::string& string, std::vector<uint32_t>& codepoints)
{
    codepoints.reserve(string.size());

    try
    {
        utf8::iterator<std::string::const_iterator> i(string.begin(), string.begin(), string.end());
        utf8::iterator<std::string::const_iterator> end(string.end(), string.begin(), string.end());

        while (i != end)
        {
            uint32_t g = *i++;
            codepoints.push_back(g);
        }
    }
    catch (utf8::exception& e)
    {
        throw love::Exception("UTF-8 decoding error: %s", e.what());
    }
}

void love::GetCodepointsFromString(const std::vector<ColoredString>& strings,
                                   ColoredCodepoints& codepoints)
{
    if (strings.empty())
        return;

    codepoints.cps.reserve(strings[0].str.size());

    for (const ColoredString& cstr : strings)
    {
        // No need to add the color if the string is empty anyway, and the code
        // further on assumes no two colors share the same starting position.
        if (cstr.str.size() == 0)
            continue;

        IndexedColor c = { cstr.color, (int)codepoints.cps.size() };
        codepoints.colors.push_back(c);

        GetCodepointsFromString(cstr.str, codepoints.cps);
    }

    if (codepoints.colors.size() == 1)
    {
        IndexedColor c = codepoints.colors[0];

        if (c.index == 0 && c.color == Color(1.0f, 1.0f, 1.0f, 1.0f))
            codepoints.colors.pop_back();
    }
}

Type TextShaper::type("TextShaper", &Object::type);

TextShaper::TextShaper(Rasterizer* rasterizer) :
    rasterizers { rasterizer },
    dpiScales { rasterizer->GetDPIScale() },
    height(std::floor(rasterizer->GetHeight() / rasterizer->GetDPIScale() + 0.5f)),
    lineHeight(1.0f),
    useSpacesForTab(false)
{
    if (!rasterizer->HasGlyph('\t'))
        this->useSpacesForTab = true;
}

bool TextShaper::HasGlyph(uint32_t glyph) const
{
    for (const auto& rasterizer : this->rasterizers)
    {
        if (rasterizer->HasGlyph(glyph))
            return true;
    }

    return false;
}

bool TextShaper::HasGlyphs(const std::string& text) const
{
    if (text.size() == 0)
        return false;

    try
    {
        utf8::iterator<std::string::const_iterator> i(text.begin(), text.begin(), text.end());
        utf8::iterator<std::string::const_iterator> end(text.end(), text.begin(), text.end());

        while (i != end)
        {
            uint32_t codepoint = *i++;

            if (!this->HasGlyph(codepoint))
                return false;
        }
    }
    catch (utf8::exception& e)
    {
        throw love::Exception("UTF-8 decoding error: %s", e.what());
    }
}

int TextShaper::GetAscent() const
{
    return std::floor(this->rasterizers[0]->GetAscent() / this->rasterizers[0]->GetDPIScale() +
                      0.5f);
}

int TextShaper::GetDescent() const
{
    return std::floor(this->rasterizers[0]->GetDescent() / this->rasterizers[0]->GetDPIScale() +
                      0.5f);
}

float TextShaper::GetBaseline() const
{
    float ascent = this->GetAscent();

    if (ascent != 0.0f)
        return ascent;
    else if (this->rasterizers[0]->GetDataType() == Rasterizer::DATA_TRUETYPE)
        return std::floor(this->GetHeight() / 1.25f + 0.5f);
    else
        return 0.0f;
}

float TextShaper::GetKerning(uint32_t leftGlyph, uint32_t rightGlyph)
{
    uint64_t packedGlyphs = ((uint64_t)leftGlyph << 32) | (uint64_t)rightGlyph;

    const auto iterator = this->kernings.find(packedGlyphs);
    if (iterator != this->kernings.end())
        return iterator->second;

    float kerning = 0.0f;
    bool found    = false;

    for (const auto& rasterizer : this->rasterizers)
    {
        if (rasterizer->HasGlyph(leftGlyph) && rasterizer->HasGlyph(rightGlyph))
        {
            found   = true;
            kerning = std::floor(
                rasterizer->GetKerning(leftGlyph, rightGlyph) / rasterizer->GetDPIScale() + 0.5f);
            break;
        }
    }

    if (!found)
    {
        float _kerning = std::floor(this->rasterizers[0]->GetKerning(leftGlyph, rightGlyph));
        kerning        = _kerning / this->rasterizers[0]->GetDPIScale() + 0.5f;
    }

    this->kernings[packedGlyphs] = kerning;
    return kerning;
}

float TextShaper::GetKerning(const std::string& left, const std::string& right)
{
    uint32_t leftGlyph  = 0;
    uint32_t rightGlyph = 0;

    try
    {
        leftGlyph  = utf8::peek_next(left.begin(), left.end());
        rightGlyph = utf8::peek_next(right.begin(), right.end());
    }
    catch (utf8::exception& e)
    {
        throw love::Exception("UTF-8 decoding error: %s", e.what());
    }
}

int TextShaper::GetGlyphAdvance(uint32_t glyph, GlyphIndex* index)
{
    const auto iterator = this->glyphAdvances.find(glyph);
    if (iterator != this->glyphAdvances.end())
    {
        if (index)
            *index = iterator->second.second;

        return iterator->second.first;
    }

    int rasterizerIndex = 0;
    uint32_t realGlyph  = glyph;

    if (glyph == '\t' && this->IsUsingSpacesForTab())
        realGlyph = ' ';

    for (size_t index = 0; index < this->rasterizers.size(); index++)
    {
        if (this->rasterizers[index]->HasGlyph(realGlyph))
        {
            rasterizerIndex = index;
            break;
        }
    }

    const auto& rasterizer = this->rasterizers[rasterizerIndex];
    int advance =
        std::floor(rasterizer->GetGlyphSpacing(realGlyph) / rasterizer->GetDPIScale() + 0.5f);

    if (glyph == '\t' && realGlyph == ' ')
        advance *= TextShaper::SPACES_PER_TAB;

    GlyphIndex glyphIndex      = { rasterizer->GetGlyphIndex(realGlyph), rasterizerIndex };
    this->glyphAdvances[glyph] = std::make_pair(advance, glyphIndex);

    if (index)
        *index = glyphIndex;

    return advance;
}

int TextShaper::GetWidth(const std::string& string)
{
    if (string.size() == 0)
        return 0;

    ColoredCodepoints codepoints {};
    love::GetCodepointsFromString(string, codepoints.cps);

    TextInfo info {};
    this->ComputeGlyphPositions(codepoints, Range(), Vector2(), 0.0f, nullptr, nullptr, &info);

    return info.width;
}

static size_t findNewLine(const ColoredCodepoints& codepoints, size_t start)
{
    for (size_t index = start; index < codepoints.cps.size(); index++)
    {
        if (codepoints.cps[index] == '\n')
            return index;
    }

    return codepoints.cps.size();
}

void TextShaper::GetWrap(const ColoredCodepoints& codepoints, float wrapLimit,
                         std::vector<Range>& lineRanges, std::vector<int>* lineWidths)
{
    size_t nextNewline = findNewLine(codepoints, 0);

    for (size_t index = 0; index < codepoints.cps.size(); index++)
    {
        if (nextNewline < index)
            nextNewline = findNewLine(codepoints, index);

        if (nextNewline == index)
        {
            lineRanges.push_back(Range());

            if (lineWidths)
                lineWidths->push_back(0);

            index++;
        }
        else
        {
            Range range(index, nextNewline - index);
            float width = 0.0f;

            int wrapIndex = this->ComputeWordWrapIndex(codepoints, range, wrapLimit, &width);

            if (wrapIndex > (int)index)
            {
                range = Range(index, (size_t)wrapIndex - index);
                index = (size_t)wrapIndex;
            }
            else
            {
                range = Range();
                index++;
            }

            if (nextNewline == index)
                index++;

            lineRanges.push_back(range);

            if (lineWidths)
                lineWidths->push_back(width);
        }
    }
}

void TextShaper::GetWrap(const std::vector<ColoredString>& text, float wrapLimit,
                         std::vector<std::string>& lines, std::vector<int>* lineWidths)
{
    ColoredCodepoints codepoints {};
    love::GetCodepointsFromString(text, codepoints);

    std::vector<Range> codepointRanges {};
    this->GetWrap(codepoints, wrapLimit, codepointRanges, lineWidths);

    std::string line {};
    for (const auto& range : codepointRanges)
    {
        line.clear();

        if (range.isValid())
        {
            line.reserve(range.getSize());

            for (size_t index = range.getMin(); index <= range.getMax(); index++)
            {
                char character[5] = { '\0' };
                char* end         = utf8::unchecked::append(codepoints.cps[index], character);
                line.append(character, end - character);
            }
        }

        lines.push_back(line);
    }
}

void TextShaper::SetFallbacks(const std::vector<Rasterizer*>& fallbacks)
{
    for (auto* rasterizer : fallbacks)
    {
        if (rasterizer->GetDataType() != this->rasterizers[0]->GetDataType())
            throw love::Exception("Font fallbacks must be of the same font type.");

        this->kernings.clear();
        this->glyphAdvances.clear();

        this->rasterizers.resize(1);
        this->dpiScales.resize(1);

        for (auto* rasterizer : fallbacks)
        {
            this->rasterizers.push_back(rasterizer);
            this->dpiScales.push_back(rasterizer->GetDPIScale());
        }
    }
}