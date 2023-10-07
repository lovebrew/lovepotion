#include <modules/window_ext.hpp>

#include <modules/fontmodule_ext.hpp>

using namespace love;

FontModule<Console::HAC>::FontModule() : FontModule<Console::ALL>()
{
    this->defaultFontData.Set(new SystemFont(), Acquire::NORETAIN);
}

Rasterizer* FontModule<Console::HAC>::NewRasterizer(FileData* data) const
{
    if (TrueTypeRasterizer<Console::HAC>::Accepts(this->library, data))
        return this->NewTrueTypeRasterizer(data, 12, TrueTypeRasterizer<>::HINTING_NORMAL);

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