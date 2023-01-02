#include <modules/graphics_ext.hpp>
#include <utilities/driver/renderer_ext.hpp>

#include <common/matrix_ext.hpp>

#include <modules/fontmodule_ext.hpp>
#include <modules/window_ext.hpp>

#include <objects/texture_ext.hpp>

using Renderer = love::Renderer<love::Console::CAFE>;
using namespace love;

Graphics<Console::CAFE>::Graphics()
{
    auto* window = Module::GetInstance<Window<Console::CAFE>>(Module::M_WINDOW);

    if (window != nullptr)
    {
        window->SetGraphics(this);

        if (window->IsOpen())
            window->SetWindow();
    }
}

void Graphics<Console::CAFE>::Clear(OptionalColor color, OptionalInt stencil, OptionalDouble depth)
{
    ::Renderer::Instance().BindFramebuffer();

    if (color.has_value())
    {
        Graphics<Console::ALL>::GammaCorrectColor(color.value());
        ::Renderer::Instance().Clear(color.value());
    }

    if (stencil.has_value() && depth.has_value())
        ::Renderer::Instance().ClearDepthStencil(stencil.value(), 0xFF, depth.value());
}

void Graphics<Console::CAFE>::Clear(std::vector<OptionalColor>& colors, OptionalInt stencil,
                                    OptionalDouble depth)
{
    int colorCount = colors.size();

    if (colorCount == 0 || !stencil.has_value() || !depth.has_value())
        this->Clear(colorCount > 0 ? colors[0] : Color {}, stencil, depth);
}

void Graphics<Console::CAFE>::Present()
{
    ::Renderer::Instance().Present();
}

void Graphics<Console::CAFE>::Reset()
{
    DisplayState state {};
    this->RestoreState(state);
    Graphics<>::Reset();
}

void Graphics<Console::CAFE>::RestoreState(const DisplayState& state)
{
    Graphics<>::RestoreState(state);

    if (state.scissor.active)
        this->SetScissor(state.scissor.bounds);
    else
        Graphics<>::SetScissor();

    this->SetColor(state.foreground);
    this->SetBlendState(state.blendState);
    this->SetShader(state.shader.Get());
}

void Graphics<Console::CAFE>::SetBlendMode(RenderState::BlendMode mode,
                                           RenderState::BlendAlpha alphaMode)
{
    Graphics<>::SetBlendMode(mode, alphaMode);
    this->SetBlendState(love::RenderState::ComputeBlendState(mode, alphaMode));
}

void Graphics<Console::CAFE>::SetBlendState(const RenderState::BlendState& state)
{
    Renderer<Console::CAFE>::Instance().SetBlendMode(state);
    states.back().blendState = state;
}

void Graphics<Console::CAFE>::RestoreStateChecked(const DisplayState& state)
{
    Graphics<>::RestoreStateChecked(state);

    const DisplayState& current = this->states.back();

    bool sameScissor = state.scissor.bounds == current.scissor.bounds;
    if (state.scissor.active != current.scissor.active || (state.scissor.active && !sameScissor))
    {
        if (state.scissor.active)
            this->SetScissor(state.scissor.bounds);
        else
            Graphics<>::SetScissor();
    }

    if (state.foreground != current.foreground)
        this->SetColor(state.foreground);

    if (!(state.blendState == current.blendState))
        this->SetBlendState(state.blendState);

    this->SetShader(state.shader.Get());
}

void Graphics<Console::CAFE>::SetColor(const Color& color)
{
    Graphics<>::SetColor(color);
    Renderer<Console::CAFE>::Instance().SetBlendColor(color);
}

int Graphics<Console::CAFE>::GetWidth(Screen screen) const
{
    return Renderer<Console::CAFE>::Instance().GetFrameBufferSize(screen).x;
}

int Graphics<Console::CAFE>::GetHeight(Screen screen) const
{
    return Renderer<Console::CAFE>::Instance().GetFrameBufferSize(screen).y;
}

bool Graphics<Console::CAFE>::SetMode(int x, int y, int width, int height)
{
    ::Renderer::Instance();
    this->RestoreState(this->states.back());

    this->SetViewportSize(width, height);

    for (int index = 0; index < Shader<>::STANDARD_MAX_ENUM; index++)
    {
        const auto type = (Shader<>::StandardShader)index;

        try
        {
            if (!Shader<Console::CAFE>::defaults[index])
            {
                auto* shader = new Shader<Console::CAFE>();
                shader->LoadDefaults(type);

                Shader<Console::CAFE>::defaults[index] = shader;
            }
        }
        catch (love::Exception&)
        {
            throw;
        }
    }

    if (!Shader<Console::CAFE>::current)
        Shader<Console::CAFE>::defaults[Shader<>::STANDARD_DEFAULT]->Attach();

    this->created = true;
    return true;
}

void Graphics<Console::CAFE>::CheckSetDefaultFont()
{
    /* don't assign if a font exists */
    if (states.back().font.Get() != nullptr)
        return;

    /*/ Create a new default font if we don't have one yet. */
    if (!defaultFont.Get())
    {
        const auto hinting = Rasterizer<Console::CAFE>::HINTING_NORMAL;
        this->defaultFont.Set(this->NewDefaultFont(13, hinting), Acquire::NORETAIN);
    }

    states.back().font.Set(this->defaultFont.Get());
}

void Graphics<Console::CAFE>::Pop()
{
    Graphics<>::Pop();

    if (this->stackTypeStack.back() == STACK_ALL)
    {
        DisplayState& newState = this->states[this->states.size() - 2];
        this->RestoreStateChecked(newState);
        this->states.pop_back();
    }

    this->stackTypeStack.pop_back();
}

Font<Console::CAFE>* Graphics<Console::CAFE>::NewFont(Rasterizer<Console::CAFE>* data) const
{
    return new Font<Console::CAFE>(data, this->GetDefaultSamplerState());
}

Font<Console::CAFE>* Graphics<Console::CAFE>::NewDefaultFont(
    int size, Rasterizer<Console::CAFE>::Hinting hinting) const
{
    auto* font = Module::GetInstance<FontModule<Console::CAFE>>(M_FONT);

    if (font == nullptr)
        throw love::Exception("Font module has not been loaded.");

    StrongReference<Rasterizer<Console::CAFE>> data(font->NewTrueTypeRasterizer(size, hinting),
                                                    Acquire::NORETAIN);

    return this->NewFont(data.Get());
}

Texture<Console::CAFE>* Graphics<Console::CAFE>::NewTexture(const Texture<>::Settings& settings,
                                                            const Texture<>::Slices* slices) const
{
    return new Texture<Console::CAFE>(this, settings, slices);
}

void Graphics<Console::CAFE>::Draw(Drawable* drawable, const Matrix4<Console::CAFE>& matrix)
{
    drawable->Draw(*this, matrix);
}

void Graphics<Console::CAFE>::Draw(Texture<Console::CAFE>* texture, Quad* quad,
                                   const Matrix4<Console::CAFE>& matrix)
{
    texture->Draw(*this, quad, matrix);
}

void Graphics<Console::CAFE>::Print(const Font<>::ColoredStrings& strings,
                                    const Matrix4<Console::CAFE>& matrix)
{
    this->CheckSetDefaultFont();

    if (this->states.back().font.Get() != nullptr)
        this->Print(strings, this->states.back().font.Get(), matrix);
}

void Graphics<Console::CAFE>::Print(const Font<>::ColoredStrings& strings,
                                    Font<Console::CAFE>* font, const Matrix4<Console::CAFE>& matrix)
{
    font->Print(*this, strings, matrix, this->states.back().foreground);
}

void Graphics<Console::CAFE>::Printf(const Font<>::ColoredStrings& strings, float wrap,
                                     Font<>::AlignMode align, const Matrix4<Console::CAFE>& matrix)
{
    this->CheckSetDefaultFont();

    if (this->states.back().font.Get() != nullptr)
        this->Printf(strings, this->states.back().font.Get(), wrap, align, matrix);
}

void Graphics<Console::CAFE>::Printf(const Font<>::ColoredStrings& strings,
                                     Font<Console::CAFE>* font, float wrap, Font<>::AlignMode align,
                                     const Matrix4<Console::CAFE>& matrix)
{
    font->Printf(*this, strings, wrap, align, matrix, this->states.back().foreground);
}

void Graphics<Console::CAFE>::SetViewportSize(int width, int height)
{
    ::Renderer::Instance().SetViewport(Rect::EMPTY);
    ::Renderer::Instance().SetScissor(Rect::EMPTY);
}

void Graphics<Console::CAFE>::SetScissor()
{
    Graphics<Console::ALL>::SetScissor();
    ::Renderer::Instance().SetScissor(Rect::EMPTY);
}

void Graphics<Console::CAFE>::SetScissor(const Rect& scissor)
{
    Graphics<Console::ALL>::SetScissor(scissor);
    ::Renderer::Instance().SetScissor(scissor);
}
