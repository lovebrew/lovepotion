#include <modules/graphics_ext.hpp>

#include <objects/rasterizer_ext.hpp>

#include <modules/fontmodule_ext.hpp>
#include <modules/window_ext.hpp>

#include <objects/font_ext.hpp>
#include <objects/textbatch_ext.hpp>
#include <objects/texture_ext.hpp>

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

/* objects */

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

Font<Console::CTR>* Graphics<Console::CTR>::NewFont(Rasterizer<Console::CTR>* data) const
{
    return new Font<Console::CTR>(data, this->states.back().defaultSamplerState);
}

TextBatch<Console::CTR>* Graphics<Console::CTR>::NewTextBatch(Font<Console::CTR>* font,
                                                              const Font<>::ColoredStrings& text)
{
    return new TextBatch<Console::CTR>(font, text);
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

void Graphics<Console::CTR>::Draw(Drawable* drawable, const Matrix4<Console::CTR>& matrix)
{
    drawable->Draw(*this, matrix);
}

void Graphics<Console::CTR>::Draw(Texture<Console::CTR>* texture, Quad* quad,
                                  const Matrix4<Console::CTR>& matrix)
{
    texture->Draw(*this, quad, matrix);
}

Texture<Console::CTR>* Graphics<Console::CTR>::NewTexture(const Texture<>::Settings& settings,
                                                          const Texture<>::Slices* slices) const
{
    return new Texture<Console::CTR>(this, settings, slices);
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

    this->RestoreState(this->states.back());

    for (int index = 0; index < Shader<>::STANDARD_MAX_ENUM; index++)
    {
        const auto type = (Shader<>::StandardShader)index;

        try
        {
            if (!Shader<Console::CTR>::defaults[index])
            {
                auto* shader = new Shader<Console::CTR>();
                shader->LoadDefaults(type);

                Shader<Console::CTR>::defaults[index] = shader;
            }
        }
        catch (love::Exception&)
        {
            throw;
        }
    }

    if (!Shader<Console::CTR>::current)
        Shader<Console::CTR>::defaults[Shader<>::STANDARD_DEFAULT]->Attach();

    this->created = true;
}
