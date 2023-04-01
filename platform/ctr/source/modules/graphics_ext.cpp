#include <modules/graphics_ext.hpp>

#include <objects/rasterizer_ext.hpp>

#include <modules/fontmodule_ext.hpp>
#include <modules/window_ext.hpp>

using Renderer = love::Renderer<love::Console::CTR>;
using namespace love;

Graphics<Console::CTR>::Graphics()
{
    auto* window = Module::GetInstance<Window<Console::Which>>(Module::M_WINDOW);

    if (window != nullptr)
    {
        window->SetGraphics(this);

        if (window->IsOpen())
            window->SetWindow();
    }

    this->CheckSetDefaultFont();
}

Font<Console::CTR>* Graphics<Console::CTR>::NewDefaultFont(int size) const
{
    auto fontModule = Module::GetInstance<FontModule<Console::CTR>>(M_FONT);

    if (!fontModule)
        throw love::Exception("Font module has not been loaded.");

    StrongReference<Rasterizer<Console::CTR>> rasterizer(fontModule->NewBCFNTRasterizer(size),
                                                         Acquire::NORETAIN);

    return this->NewFont(rasterizer.Get());
}

void Graphics<Console::CTR>::CheckSetDefaultFont()
{
    if (this->states.back().font.Get() != nullptr)
        return;

    if (!this->defaultFont.Get())
        this->defaultFont.Set(this->NewDefaultFont(12), Acquire::NORETAIN);

    this->states.back().font.Set(this->defaultFont.Get());
}

void Graphics<Console::CTR>::Set3D(bool enabled)
{
    return ::Renderer::Instance().Set3D(enabled);
}

bool Graphics<Console::CTR>::Get3D()
{
    return ::Renderer::Instance().Get3D();
}

void Graphics<Console::CTR>::SetMode(int x, int y, int width, int height)
{
    ::Renderer::Instance().CreateFramebuffers();
    this->RestoreState(this->states.back());

    this->created = true;
}

void Graphics<Console::CTR>::SetScissor()
{
    Graphics<Console::ALL>::SetScissor();
    ::Renderer::Instance().SetScissor({}, false);
}

void Graphics<Console::CTR>::SetScissor(const Rect& scissor)
{
    Graphics<Console::ALL>::SetScissor(scissor);
    ::Renderer::Instance().SetScissor(scissor, this->IsRenderTargetActive());
}
