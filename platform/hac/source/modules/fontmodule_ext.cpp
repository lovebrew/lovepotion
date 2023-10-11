#include <modules/window_ext.hpp>

#include <modules/fontmodule_ext.hpp>

#include <objects/bmfontrasterizer/bmfontrasterizer.hpp>
#include <objects/imagerasterizer/imagerasterizer.hpp>

using namespace love;

FontModule<Console::HAC>::FontModule() : FontModule<Console::ALL>()
{
    this->defaultFontData.Set(new SystemFont(), Acquire::NORETAIN);
}

Rasterizer* FontModule<Console::HAC>::NewImageRasterizer(ImageData<Console::Which>* data,
                                                         const std::string& text, int extraSpacing,
                                                         float dpiScale) const
{
    std::vector<uint32_t> glyphs {};
    glyphs.reserve(text.size());

    try
    {
        utf8::iterator<std::string::const_iterator> it(text.begin(), text.begin(), text.end());
        utf8::iterator<std::string::const_iterator> end(text.end(), text.begin(), text.end());

        while (it != end)
            glyphs.push_back(*it++);
    }
    catch (utf8::exception& e)
    {
        throw love::Exception("UTF-8 decoding error: %s", e.what());
    }

    return this->NewImageRasterizer(data, &glyphs[0], (int)glyphs.size(), extraSpacing, dpiScale);
}

Rasterizer* FontModule<Console::HAC>::NewImageRasterizer(ImageData<Console::Which>* data,
                                                         uint32_t* glyphs, int glyphCount,
                                                         int extraSpacing, float dpiScale) const
{
    return new ImageRasterizer(data, glyphs, glyphCount, extraSpacing, dpiScale);
}

Rasterizer* FontModule<Console::HAC>::NewRasterizer(FileData* data) const
{
    if (TrueTypeRasterizer<Console::HAC>::Accepts(this->library, data))
        return this->NewTrueTypeRasterizer(data, 12, TrueTypeRasterizer<>::HINTING_NORMAL);
    else if (BMFontRasterizer::Accepts(data))
        return this->NewBMFontRasterizer(data, {}, 1.0f);

    throw love::Exception("Invalid font file: %s", data->GetFilename().c_str());
    return nullptr;
}

Rasterizer* FontModule<Console::HAC>::NewTrueTypeRasterizer(int size,
                                                            TrueTypeRasterizer<>::Hinting hinting,
                                                            PlSharedFontType type) const
{
    StrongReference<SystemFont> data(new SystemFont(type), Acquire::NORETAIN);
    return this->NewTrueTypeRasterizer(data.Get(), size, hinting);
}

Rasterizer* FontModule<Console::HAC>::NewTrueTypeRasterizer(
    Data* data, int size, TrueTypeRasterizer<>::Hinting hinting) const
{
    float dpiScale = 1.0f;
    auto window    = Module::GetInstance<Window<Console::Which>>(Module::M_WINDOW);

    if (window != nullptr)
        dpiScale = window->GetDPIScale();

    return this->NewTrueTypeRasterizer(data, size, dpiScale, hinting);
}

Rasterizer* FontModule<Console::HAC>::NewTrueTypeRasterizer(
    Data* data, int size, float dpiScale, TrueTypeRasterizer<>::Hinting hinting) const
{
    return new TrueTypeRasterizer<Console::HAC>(this->library, data, size, dpiScale, hinting);
}