#include <objects/font_ext.hpp>

#include <modules/graphics_ext.hpp>

#include <utilities/driver/renderer_ext.hpp>

#include <utilities/functions.hpp>

#include <algorithm>
#include <numeric>

using namespace love;

Font<Console::CTR>::Font(Rasterizer<Console::CTR>* rasterizer, const SamplerState& state) :
    buffer {}
{
    this->buffer      = C2D_TextBufNew(Font::TEXT_BUFFER_SIZE);
    this->rasterizers = std::vector<StrongRasterizer>({ rasterizer });
    this->height      = this->rasterizers[0]->GetHeight();

    const auto minFilter = Renderer<Console::CTR>::filterModes.Find(state.minFilter);
    const auto magFilter = Renderer<Console::CTR>::filterModes.Find(state.magFilter);

    C2D_FontSetFilter(this->rasterizers[0]->GetFont(), *magFilter, *minFilter);
}

Font<Console::CTR>::~Font()
{
    if (!this->buffer)
        return;

    C2D_TextBufClear(this->buffer);
    C2D_TextBufDelete(this->buffer);
}

int Font<Console::CTR>::GetWidth(const std::string& text)
{
    if (text.size() == 0)
        return 0;

    int maxWidth = 0;

    long position = 0;
    int start     = 0;

    while ((position = love::get_line(text, start)) != std::string::npos)
    {
        int width        = 0;
        std::string line = text.substr(start, (position - start));

        try
        {
            Utf8Iterator begin(line.begin(), line.begin(), line.end());
            Utf8Iterator end(line.end(), line.begin(), line.end());

            while (begin != end)
            {
                uint32_t codepoint = *begin++;

                if (codepoint == '\r')
                    continue;

                width += this->GetWidth(codepoint);
            }

            start += position + 1;
        }
        catch (utf8::exception& e)
        {
            throw love::Exception("UTF-8 decoding error: %s", e.what());
        }

        maxWidth = std::max(maxWidth, width);
    }

    return maxWidth;
}

int Font<Console::CTR>::GetWidth(uint32_t glyph)
{
    auto found = this->glyphWidths.find(glyph);

    if (found != this->glyphWidths.end())
        return found->second;

    GlyphData* glyphData     = this->rasterizers[0]->GetGlyphData(glyph);
    this->glyphWidths[glyph] = glyphData->GetAdvance();

    return this->glyphWidths[glyph];
}

bool Font<Console::CTR>::HasGlyph(uint32_t glyph) const
{
    for (auto& rasterizer : this->rasterizers)
    {
        if (rasterizer->HasGlyph(glyph))
            return true;
    }

    return false;
}

bool Font<Console::CTR>::HasGlyphs(const std::string& text) const
{
    if (text.size() == 0)
        return false;

    try
    {
        Utf8Iterator begin(text.begin(), text.begin(), text.end());
        Utf8Iterator end(text.end(), text.begin(), text.end());

        while (begin != end)
        {
            uint32_t codepoint = *begin++;

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

static std::string stringify(const std::string& string, const Font<>::ColoredString& piece)
{
    return string + piece.string;
}

void Font<Console::CTR>::Print(Graphics<Console::CTR>& graphics,
                               std::vector<Font<>::ColoredString>& string,
                               const Matrix4<Console::CTR>& localTransform, const Color& color)
{
    C2D_Text text {};
    auto result = std::accumulate(string.begin(), string.end(), std::string {}, stringify);

    const auto font     = this->rasterizers[0]->GetFont();
    const auto* success = C2D_TextFontParse(&text, font, this->buffer, result.c_str());

    if (!success)
        return;

    C2D_TextOptimize(&text);

    Matrix4<Console::CTR> transform(graphics.GetTransform(), localTransform);
    C2D_ViewRestore(&transform.GetElements());

    float scale    = this->rasterizers[0]->GetScale();
    uint32_t flags = C2D_MultiColor;

    const size_t limit = string.size() * 2;
    uint32_t colors[string.size() * 2] {};

    for (uint32_t index = 0; index < limit; index += 2)
    {
        const auto& value = string[index / 2];

        colors[index + 0] = result.find(value.string);
        colors[index + 1] = value.color.rgba();
    }

    C2D_DrawText(&text, flags, 0, 0, graphics.GetCurrentDepth(), scale, scale, colors, limit);
}

void Font<Console::CTR>::Printf(Graphics<Console::CTR>& graphics,
                                std::vector<ColoredString>& string, float wrap, AlignMode alignment,
                                const Matrix4<Console::CTR>& localTransform, const Color& color)
{
    C2D_Text text {};
    auto result = std::accumulate(string.begin(), string.end(), std::string {}, stringify);

    std::optional<uint32_t> mode;
    if (!(mode = Font::textWrapModes.Find(alignment)))
        mode = C2D_AlignLeft;

    float offset = 0.0f;
    if (alignment == ALIGN_CENTER)
        offset = wrap / 2.0f;

    const auto font     = this->rasterizers[0]->GetFont();
    const auto* success = C2D_TextFontParse(&text, font, this->buffer, result.c_str());

    if (!success)
        return;

    Matrix4<Console::CTR> transform(graphics.GetTransform(), localTransform);
    C2D_ViewRestore(&transform.GetElements());

    float scale    = this->rasterizers[0]->GetScale();
    uint32_t flags = C2D_MultiColor | *mode;

    const size_t limit = string.size() * 2;
    uint32_t colors[string.size() * 2] {};

    for (uint32_t index = 0; index < limit; index += 2)
    {
        const auto& value = string[index / 2];

        colors[index + 0] = result.find(value.string);
        colors[index + 1] = value.color.rgba();
    }

    C2D_DrawText(&text, flags, offset, 0, graphics.GetCurrentDepth(), scale, scale, wrap, colors,
                 limit);
}

float Font<Console::CTR>::GetKerning(const std::string&, const std::string&) const
{
    return 0.0f;
}

float Font<Console::CTR>::GetKerning(uint32_t, uint32_t) const
{
    return 0.0f;
}

void Font<Console::CTR>::SetSamplerState(const SamplerState& state)
{
    Font<>::SetSamplerState(state);

    const auto minFilter = Renderer<Console::CTR>::filterModes.Find(state.minFilter);
    const auto magFilter = Renderer<Console::CTR>::filterModes.Find(state.magFilter);

    C2D_FontSetFilter(this->rasterizers[0]->GetFont(), *magFilter, *minFilter);
}
