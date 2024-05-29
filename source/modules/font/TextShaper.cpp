#include "common/Exception.hpp"

#include "modules/font/Rasterizer.hpp"
#include "modules/font/TextShaper.hpp"

#include "utf8.h"

namespace love
{
    void getCodepointsFromString(const std::string& text, std::vector<uint32_t>& codepoints)
    {
        codepoints.reserve(text.size());

        try
        {
            utf8::iterator<std::string::const_iterator> i(text.begin(), text.begin(), text.end());
            utf8::iterator<std::string::const_iterator> end(text.end(), text.begin(), text.end());

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

    void getCodepointsFromString(const std::vector<ColoredString>& strs, ColoredCodepoints& codepoints)
    {
        if (strs.empty())
            return;

        codepoints.codepoints.reserve(strs[0].string.size());

        for (const ColoredString& cstr : strs)
        {
            // No need to add the color if the string is empty anyway, and the code
            // further on assumes no two colors share the same starting position.
            if (cstr.string.size() == 0)
                continue;

            IndexedColor c = { cstr.color, (int)codepoints.codepoints.size() };
            codepoints.colors.push_back(c);

            getCodepointsFromString(cstr.string, codepoints.codepoints);
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
        dpiScales { rasterizer->getDPIScale() },
        height(std::floor(rasterizer->getHeight() / rasterizer->getDPIScale() + 0.5f)),
        lineHeight(1),
        useSpacesForTab(false)
    {
        if (!rasterizer->hasGlyph('\t'))
            this->useSpacesForTab = true;
    }

    TextShaper::~TextShaper()
    {}

    int TextShaper::getAscent() const
    {
        return std::floor(this->rasterizers[0]->getAscent() / rasterizers[0]->getDPIScale() + 0.5f);
    }

    int TextShaper::getDescent() const
    {
        return std::floor(this->rasterizers[0]->getDescent() / rasterizers[0]->getDPIScale() + 0.5f);
    }

    float TextShaper::getBaseline() const
    {
        float ascent = this->getAscent();

        if (ascent != 0.0f)
            return ascent;
        else if (rasterizers[0]->getDataType() == Rasterizer::DATA_TRUETYPE)
            return std::floor(this->getHeight() / 1.25f + 0.5f);
        else
            return 0.0f;
    }

    bool TextShaper::hasGlyph(uint32_t glyph) const
    {
        for (const auto& rasterizer : this->rasterizers)
        {
            if (rasterizer->hasGlyph(glyph))
                return true;
        }

        return false;
    }

    bool TextShaper::hasGlyphs(const std::string& text) const
    {
        if (text.size() == 0)
            return false;

        try
        {
            utf8::iterator<std::string::const_iterator> it(text.begin(), text.begin(), text.end());
            utf8::iterator<std::string::const_iterator> end(text.end(), text.begin(), text.end());

            while (it != end)
            {
                uint32_t glyph = *it++;

                if (!this->hasGlyph(glyph))
                    return false;
            }
        }
        catch (utf8::exception& e)
        {
            throw love::Exception("UTF-8 decoding error: %s", e.what());
        }

        return true;
    }

    float TextShaper::getKerning(uint32_t leftglyph, uint32_t rightglyph)
    {
        if (this->rasterizers[0]->getDataType() == Rasterizer::DATA_BCFNT)
            return 0.0f;

        uint64_t packedglyphs = ((uint64_t)leftglyph << 32) | (uint64_t)rightglyph;

        const auto it = this->kernings.find(packedglyphs);
        if (it != this->kernings.end())
            return it->second;

        float result = 0.0f;
        bool found   = false;

        for (const auto& r : rasterizers)
        {
            if (r->hasGlyph(leftglyph) && r->hasGlyph(rightglyph))
            {
                found  = true;
                result = std::floor(r->getKerning(leftglyph, rightglyph) / r->getDPIScale() + 0.5f);
                break;
            }
        }

        if (!found)
        {
            auto kerning = rasterizers[0]->getKerning(leftglyph, rightglyph);
            result       = std::floor(kerning / this->rasterizers[0]->getDPIScale() + 0.5f);
        }

        this->kernings[packedglyphs] = result;
        return result;
    }

    float TextShaper::getKerning(const std::string& leftchar, const std::string& rightchar)
    {
        uint32_t left  = 0;
        uint32_t right = 0;

        try
        {
            left  = utf8::peek_next(leftchar.begin(), leftchar.end());
            right = utf8::peek_next(rightchar.begin(), rightchar.end());
        }
        catch (utf8::exception& e)
        {
            throw love::Exception("UTF-8 decoding error: %s", e.what());
        }

        return this->getKerning(left, right);
    }

    float TextShaper::getGlyphAdvance(uint32_t glyph, GlyphIndex* glyphindex)
    {
        const auto it = this->glyphAdvances.find(glyph);
        if (it != this->glyphAdvances.end())
        {
            if (glyphindex)
                *glyphindex = it->second.second;

            return it->second.first;
        }

        int rasterizerIndex = 0;
        uint32_t realGlyph  = glyph;

        if (glyph == '\t' && isUsingSpacesForTab())
            realGlyph = ' ';

        for (size_t i = 0; i < this->rasterizers.size(); i++)
        {
            if (this->rasterizers[i]->hasGlyph(realGlyph))
            {
                rasterizerIndex = (int)i;
                break;
            }
        }

        const auto& r = this->rasterizers[rasterizerIndex];
        int advance   = floorf(r->getGlyphSpacing(realGlyph) / r->getDPIScale() + 0.5f);

        if (glyph == '\t' && realGlyph == ' ')
            advance *= SPACES_PER_TAB;

        GlyphIndex glyphi = { r->getGlyphIndex(realGlyph), rasterizerIndex };

        this->glyphAdvances[glyph] = std::make_pair(advance, glyphi);

        if (glyphindex)
            *glyphindex = glyphi;

        return advance;
    }

    int TextShaper::getWidth(const std::string& str)
    {
        if (str.size() == 0)
            return 0;

        ColoredCodepoints codepoints {};
        getCodepointsFromString(str, codepoints.codepoints);

        TextInfo info {};
        this->computeGlyphPositions(codepoints, Range(), Vector2(0.0f, 0.0f), 0.0f, nullptr, nullptr, &info);

        return info.width;
    }

    static size_t findNewline(const ColoredCodepoints& codepoints, size_t start)
    {
        for (size_t i = start; i < codepoints.codepoints.size(); i++)
        {
            if (codepoints.codepoints[i] == '\n')
                return i;
        }

        return codepoints.codepoints.size();
    }

    void TextShaper::getWrap(const ColoredCodepoints& codepoints, float wraplimit,
                             std::vector<Range>& lineranges, std::vector<int>* linewidths)
    {
        size_t nextNewline = findNewline(codepoints, 0);

        for (size_t i = 0; i < codepoints.codepoints.size();)
        {
            if (nextNewline < i)
                nextNewline = findNewline(codepoints, i);

            if (nextNewline == i) // Empty line.
            {
                lineranges.push_back(Range());
                if (linewidths)
                    linewidths->push_back(0);
                i++;
            }
            else
            {
                Range r(i, nextNewline - i);
                float width   = 0.0f;
                int wrapindex = this->computeWordWrapIndex(codepoints, r, wraplimit, &width);

                if (wrapindex > (int)i)
                {
                    r = Range(i, (size_t)wrapindex - i);
                    i = (size_t)wrapindex;
                }
                else
                {
                    r = Range();
                    i++;
                }

                // We've already handled this line, skip the newline character.
                if (nextNewline == i)
                    i++;

                lineranges.push_back(r);
                if (linewidths)
                    linewidths->push_back(width);
            }
        }
    }

    void TextShaper::getWrap(const std::vector<ColoredString>& text, float wraplimit,
                             std::vector<std::string>& lines, std::vector<int>* linewidths)
    {
        ColoredCodepoints codepoints;
        getCodepointsFromString(text, codepoints);

        std::vector<Range> codepointranges;
        getWrap(codepoints, wraplimit, codepointranges, linewidths);

        std::string line;

        for (const auto& range : codepointranges)
        {
            line.clear();

            if (range.isValid())
            {
                line.reserve(range.getSize());

                for (size_t i = range.getMin(); i <= range.getMax(); i++)
                {
                    char character[5] = { '\0' };
                    char* end         = utf8::unchecked::append(codepoints.codepoints[i], character);
                    line.append(character, end - character);
                }
            }

            lines.push_back(line);
        }
    }

    void TextShaper::setFallbacks(const std::vector<Rasterizer*>& fallbacks)
    {
        for (Rasterizer* r : fallbacks)
        {
            if (r->getDataType() != rasterizers[0]->getDataType())
                throw love::Exception("Font fallbacks must be of the same font type.");
        }

        // Clear caches.
        this->kernings.clear();
        this->glyphAdvances.clear();

        this->rasterizers.resize(1);
        this->dpiScales.resize(1);

        for (Rasterizer* r : fallbacks)
        {
            this->rasterizers.push_back(r);
            this->dpiScales.push_back(r->getDPIScale());
        }
    }
} // namespace love
