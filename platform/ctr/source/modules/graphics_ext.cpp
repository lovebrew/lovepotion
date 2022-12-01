#include <modules/graphics_ext.hpp>

#include <objects/font_ext.hpp>
#include <objects/rasterizer_ext.hpp>

#include <utilities/driver/renderer_ext.hpp>

#include <modules/fontmodule_ext.hpp>
#include <modules/window_ext.hpp>

using Renderer = love::Renderer<love::Console::CTR>;
using namespace love;

Graphics<Console::CTR>::Graphics()
{
    auto* window = Module::GetInstance<Window<Console::CTR>>(Module::M_WINDOW);

    if (window != nullptr)
    {
        window->SetGraphics(this);

        if (window->IsOpen())
            window->SetWindow();
    }
}

void Graphics<Console::CTR>::Clear(OptionalColor color, OptionalInt stencil, OptionalDouble depth)
{
    ::Renderer::Instance().BindFramebuffer();

    if (color.has_value())
    {
        /* todo: Graphics<>::GammaCorrectColor(color); */
        ::Renderer::Instance().Clear(color.value());
    }

    if (stencil.has_value() && depth.has_value())
        ::Renderer::Instance().ClearDepthStencil(stencil.value(), 0xFF, depth.value());
}

void Graphics<Console::CTR>::Clear(std::vector<OptionalColor>& colors, OptionalInt stencil,
                                   OptionalDouble depth)
{
    int colorCount = colors.size();

    if (colorCount == 0 || !stencil.has_value() || !depth.has_value())
        this->Clear(colorCount > 0 ? colors[0] : Color {}, stencil, depth);
}

void Graphics<Console::CTR>::Present()
{
    ::Renderer::Instance().Present();
}

Font<Console::CTR>* Graphics<Console::CTR>::NewFont(Rasterizer<Console::CTR>* data)
{
    return new Font<Console::CTR>(data, this->states.back().defaultSamplerState);
}

Font<Console::CTR>* Graphics<Console::CTR>::NewDefaultFont(int size, CFG_Region region)
{
    auto fontModule = Module::GetInstance<FontModule<Console::CTR>>(M_FONT);

    if (!fontModule)
        throw love::Exception("Font module has not been loaded.");

    StrongReference<Rasterizer<Console::CTR>> rasterizer(
        fontModule->NewBCFNTRasterizer(size, region), Acquire::NORETAIN);

    return this->NewFont(rasterizer.Get());
}

void Graphics<Console::CTR>::CheckSetDefaultFont()
{
    if (this->states.back().font.Get() != nullptr)
        return;

    if (!this->defaultFont.Get())
        this->defaultFont.Set(this->NewDefaultFont(12, CFG_REGION_USA), Acquire::NORETAIN);

    this->states.back().font.Set(this->defaultFont.Get());
}

Font<Console::CTR>* Graphics<Console::CTR>::GetFont()
{
    this->CheckSetDefaultFont();
    return this->states.back().font.Get();
}

void Graphics<Console::CTR>::Print(const Font<>::ColoredStrings& strings,
                                   const Matrix4<Console::CTR>& matrix)
{
    this->CheckSetDefaultFont();

    if (this->states.back().font.Get() != nullptr)
        this->Print(strings, this->states.back().font.Get(), matrix);
}

void Graphics<Console::CTR>::Print(const Font<>::ColoredStrings& strings, Font<Console::CTR>* font,
                                   const Matrix4<Console::CTR>& matrix)
{
    font->Print(*this, strings, matrix, this->states.back().foreground);
}

void Graphics<Console::CTR>::Printf(const Font<>::ColoredStrings& strings, float wrap,
                                    Font<>::AlignMode align, const Matrix4<Console::CTR>& matrix)
{
    this->CheckSetDefaultFont();

    if (this->states.back().font.Get() != nullptr)
        this->Printf(strings, this->states.back().font.Get(), wrap, align, matrix);
}

void Graphics<Console::CTR>::Printf(const Font<>::ColoredStrings& strings, Font<Console::CTR>* font,
                                    float wrap, Font<>::AlignMode align,
                                    const Matrix4<Console::CTR>& matrix)
{
    font->Printf(*this, strings, wrap, align, matrix, this->states.back().foreground);
}

void Graphics<Console::CTR>::SetMode(int x, int y, int width, int height)
{
    ::Renderer::Instance().CreateFramebuffers();
    this->RestoreState(this->states.back());
}

int Graphics<Console::CTR>::GetWidth(Screen screen) const
{
    switch (screen)
    {
        case Screen::SCREEN_LEFT:
        case Screen::SCREEN_RIGHT:
        default:
            return 400;
        case Screen::SCREEN_BOTTOM:
            return 320;
    }
}

int Graphics<Console::CTR>::GetHeight() const
{
    return 240;
}
