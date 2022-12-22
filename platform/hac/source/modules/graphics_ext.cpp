#include <modules/graphics_ext.hpp>

#include <modules/fontmodule_ext.hpp>
#include <modules/window_ext.hpp>

#include <objects/texture_ext.hpp>

#include <utilities/driver/renderer_ext.hpp>

using Renderer = love::Renderer<love::Console::HAC>;
using namespace love;

Graphics<Console::HAC>::Graphics()
{
    auto* window = Module::GetInstance<Window<Console::HAC>>(Module::M_WINDOW);

    if (window != nullptr)
    {
        window->SetGraphics(this);

        if (window->IsOpen())
        {
            int width, height;
            Window<>::WindowSettings settings {};

            window->GetWindow(width, height, settings);
            window->SetWindow(width, height, &settings);
        }
    }
}

Graphics<Console::HAC>::~Graphics()
{
    for (size_t index = 0; index < Shader<>::STANDARD_MAX_ENUM; index++)
    {
        if (Shader<Console::HAC>::defaults[index])
        {
            Shader<Console::HAC>::defaults[index]->Release();
            Shader<Console::HAC>::defaults[index] = nullptr;
        }
    }
}

void Graphics<Console::HAC>::Clear(OptionalColor color, OptionalInt stencil, OptionalDouble depth)
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

void Graphics<Console::HAC>::Clear(std::vector<OptionalColor>& colors, OptionalInt stencil,
                                   OptionalDouble depth)
{
    int colorCount = colors.size();

    if (colorCount == 0 || !stencil.has_value() || !depth.has_value())
        this->Clear(colorCount > 0 ? colors[0] : Color {}, stencil, depth);
}

void Graphics<Console::HAC>::Present()
{
    ::Renderer::Instance().Present();
}

void Graphics<Console::HAC>::Reset()
{
    DisplayState state {};
    this->RestoreState(state);
    Graphics<>::Reset();
}

void Graphics<Console::HAC>::RestoreState(const DisplayState& state)
{
    Graphics<>::RestoreState(state);
    this->SetShader(state.shader);
}

bool Graphics<Console::HAC>::SetMode(int x, int y, int width, int height)
{
    this->width  = width;
    this->height = height;

    ::Renderer::Instance().CreateFramebuffers();
    this->SetViewportSize(width, height);

    this->RestoreState(this->states.back());

    for (int index = 0; index < Shader<>::STANDARD_MAX_ENUM; index++)
    {
        const auto type = (Shader<>::StandardShader)index;

        try
        {
            if (!Shader<Console::HAC>::defaults[index])
            {
                auto* shader = new Shader<Console::HAC>();
                shader->LoadDefaults(type);

                Shader<Console::HAC>::defaults[index] = shader;
            }
        }
        catch (love::Exception&)
        {
            throw;
        }
    }

    if (!Shader<Console::HAC>::current)
        Shader<Console::HAC>::defaults[Shader<>::STANDARD_DEFAULT]->Attach();

    this->created = true;
    return true;
}

void Graphics<Console::HAC>::CheckSetDefaultFont()
{
    /* don't assign if a font exists */
    if (states.back().font.Get() != nullptr)
        return;

    /*/ Create a new default font if we don't have one yet. */
    if (!defaultFont.Get())
    {
        const auto hinting = Rasterizer<Console::HAC>::HINTING_NORMAL;
        this->defaultFont.Set(this->NewDefaultFont(13, hinting), Acquire::NORETAIN);
    }

    states.back().font.Set(this->defaultFont.Get());
}

Font<Console::HAC>* Graphics<Console::HAC>::NewFont(Rasterizer<Console::HAC>* data) const
{
    return new Font<Console::HAC>(data, this->GetDefaultSamplerState());
}

Font<Console::HAC>* Graphics<Console::HAC>::NewDefaultFont(
    int size, Rasterizer<Console::HAC>::Hinting hinting) const
{
    auto* font = Module::GetInstance<FontModule<Console::HAC>>(M_FONT);

    if (font == nullptr)
        throw love::Exception("Font module has not been loaded.");

    StrongReference<Rasterizer<Console::HAC>> data(font->NewTrueTypeRasterizer(size, hinting),
                                                   Acquire::NORETAIN);

    return this->NewFont(data.Get());
}

Texture<Console::HAC>* Graphics<Console::HAC>::NewTexture(const Texture<>::Settings& settings,
                                                          const Texture<>::Slices* slices) const
{
    return new Texture<Console::HAC>(this, settings, slices);
}

void Graphics<Console::HAC>::Draw(Drawable* drawable, const Matrix4<Console::HAC>& matrix)
{
    drawable->Draw(*this, matrix);
}

void Graphics<Console::HAC>::Draw(Texture<Console::HAC>* texture, Quad* quad,
                                  const Matrix4<Console::HAC>& matrix)
{
    texture->Draw(*this, quad, matrix);
}

void Graphics<Console::HAC>::Print(const Font<>::ColoredStrings& strings,
                                   const Matrix4<Console::HAC>& matrix)
{
    this->CheckSetDefaultFont();

    if (this->states.back().font.Get() != nullptr)
        this->Print(strings, this->states.back().font.Get(), matrix);
}

void Graphics<Console::HAC>::Print(const Font<>::ColoredStrings& strings, Font<Console::HAC>* font,
                                   const Matrix4<Console::HAC>& matrix)
{
    font->Print(*this, strings, matrix, this->states.back().foreground);
}

void Graphics<Console::HAC>::Printf(const Font<>::ColoredStrings& strings, float wrap,
                                    Font<>::AlignMode align, const Matrix4<Console::HAC>& matrix)
{
    this->CheckSetDefaultFont();

    if (this->states.back().font.Get() != nullptr)
        this->Printf(strings, this->states.back().font.Get(), wrap, align, matrix);
}

void Graphics<Console::HAC>::Printf(const Font<>::ColoredStrings& strings, Font<Console::HAC>* font,
                                    float wrap, Font<>::AlignMode align,
                                    const Matrix4<Console::HAC>& matrix)
{
    font->Printf(*this, strings, wrap, align, matrix, this->states.back().foreground);
}

void Graphics<Console::HAC>::SetViewportSize(int width, int height)
{
    ::Renderer::Instance().SetViewport({ 0, 0, width, height });
    ::Renderer::Instance().SetScissor(true, { 0, 0, width, height }, false);
}

void Graphics<Console::HAC>::SetScissor(const Rect& scissor)
{
    Graphics<Console::ALL>::SetScissor(scissor);
    ::Renderer::Instance().SetScissor(this->states.back().scissor.active, scissor, false);
}
