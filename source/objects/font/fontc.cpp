#include "objects/font/fontc.h"

#include "common/bidirectionalmap.h"
#include "utf8/utf8.h"

#include "objects/font/font.h"

namespace love::common
{
    love::Type Font::type("Font", &Object::type);

    int Font::fontCount = 0;

    Font::Font(const Texture::Filter& filter)
    {
        fontCount++;
    }

    Font::~Font()
    {
        --fontCount;
    }

    float Font::GetLineHeight() const
    {
        return this->lineHeight;
    }

    void Font::SetLineHeight(float lineHeight)
    {
        this->lineHeight = lineHeight;
    }

    Texture::Filter Font::GetFilter()
    {
        return this->filter;
    }

    int Font::GetWidth(const std::string& string)
    {
        if (string.size() == 0)
            return 0;

        int maxWidth = 0;

        try
        {
            utf8::iterator<std::string::const_iterator> i(string.begin(), string.begin(),
                                                          string.end());
            utf8::iterator<std::string::const_iterator> end(string.end(), string.begin(),
                                                            string.end());

            while (i != end)
            {
                uint32_t prevGlyph = 0;
                int width          = 0;

                for (; i != end && *i != '\n'; ++i)
                {
                    uint32_t current = *i;

                    if (current == '\r')
                        continue;

                    width += this->GetWidth(prevGlyph, current);
                    prevGlyph = current;
                }

                maxWidth = std::max(maxWidth, width);

                if (i != end)
                    ++i;
            }
        }
        catch (utf8::exception& e)
        {
            throw love::Exception("UTF-8 decoding error: %s", e.what());
        }

        return maxWidth;
    }

    bool Font::HasGlyphs(const std::string& text) const
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

        return true;
    }

    // clang-format off
constexpr auto alignModes = BidirectionalMap<>::Create(
    "left",    love::Font::AlignMode::ALIGN_LEFT,
    "right",   love::Font::AlignMode::ALIGN_RIGHT,
    "center",  love::Font::AlignMode::ALIGN_CENTER,
    "justify", love::Font::AlignMode::ALIGN_JUSTIFY
);
#if defined(__3DS__)
constexpr auto sharedFonts = BidirectionalMap<>::Create(
    "standard",  love::Font::SystemFontType::TYPE_STANDARD,
    "chinese",   love::Font::SystemFontType::TYPE_CHINESE,
    "taiwanese", love::Font::SystemFontType::TYPE_TAIWANESE,
    "korean",    love::Font::SystemFontType::TYPE_KOREAN
);
#elif defined(__SWITCH__)
constexpr auto sharedFonts = BidirectionalMap<>::Create(
    "standard",                    love::Font::SystemFontType::TYPE_STANDARD,
    "chinese simplified",          love::Font::SystemFontType::TYPE_CHINESE_SIMPLIFIED,
    "chinese traditional",         love::Font::SystemFontType::TYPE_CHINESE_TRADITIONAL,
    "extended chinese simplified", love::Font::SystemFontType::TYPE_CHINESE_SIMPLIFIED_EXT,
    "korean",                      love::Font::SystemFontType::TYPE_KOREAN,
    "nintendo extended",           love::Font::SystemFontType::TYPE_NINTENDO_EXTENDED
);
#endif
    // clang-format on

    bool Font::GetConstant(const char* in, AlignMode& out)
    {
        return alignModes.Find(in, out);
    }

    bool Font::GetConstant(AlignMode in, const char*& out)
    {
        return alignModes.ReverseFind(in, out);
    }

    std::vector<const char*> Font::GetConstants(AlignMode)
    {
        return alignModes.GetNames();
    }

    bool Font::GetConstant(const char* in, Font::SystemFontType& out)
    {
        return sharedFonts.Find(in, out);
    }

    bool Font::GetConstant(Font::SystemFontType in, const char*& out)
    {
        return sharedFonts.ReverseFind(in, out);
    }

    std::vector<const char*> Font::GetConstants(Font::SystemFontType)
    {
        return sharedFonts.GetNames();
    }
} // namespace love::common
