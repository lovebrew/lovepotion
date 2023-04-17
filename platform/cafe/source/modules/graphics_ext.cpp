#include <modules/graphics_ext.hpp>

#include <utilities/driver/renderer_ext.hpp>

#include <common/matrix_ext.hpp>

#include <modules/fontmodule_ext.hpp>
#include <modules/window_ext.hpp>

#include <objects/font_ext.hpp>
#include <objects/shader_ext.hpp>
#include <objects/textbatch_ext.hpp>
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

void Graphics<Console::CAFE>::SetFrontFaceWinding(vertex::Winding winding)
{
    Graphics<>::SetFrontFaceWinding(winding);
    Renderer<Console::CAFE>::Instance().SetVertexWinding(winding);
}

void Graphics<Console::CAFE>::SetMeshCullMode(vertex::CullMode mode)
{
    Graphics<>::SetMeshCullMode(mode);
    Renderer<Console::CAFE>::Instance().SetMeshCullMode(mode);
}

void Graphics<Console::CAFE>::SetColor(const Color& color)
{
    Graphics<>::SetColor(color);
    Renderer<Console::CAFE>::Instance().SetBlendColor(color);
}

TextBatch<Console::CAFE>* Graphics<Console::CAFE>::NewTextBatch(
    Font<Console::CAFE>* font, const Font<>::ColoredStrings& text) const
{
    return new TextBatch<Console::CAFE>(font, text);
}

bool Graphics<Console::CAFE>::SetMode(int x, int y, int width, int height)
{
    try
    {
        ::Renderer::Instance();
    }
    catch (love::Exception&)
    {
        throw;
    }

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

void Graphics<Console::CAFE>::SetShader()
{
    Shader<Console::CAFE>::AttachDefault(Shader<>::STANDARD_DEFAULT);
    this->states.back().shader.Set(nullptr);
}

void Graphics<Console::CAFE>::SetShader(Shader<Console::CAFE>* shader)
{
    if (shader == nullptr)
        return this->SetShader();

    shader->Attach();
    this->states.back().shader.Set(shader);
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

void Graphics<Console::CAFE>::IntersectScissor(const Rect& rectangle)
{
    Rect currect = states.back().scissor.bounds;

    if (!states.back().scissor.active)
    {
        currect.x = 0;
        currect.y = 0;
        currect.w = std::numeric_limits<int>::max();
        currect.h = std::numeric_limits<int>::max();
    }

    int x1 = std::max(currect.x, rectangle.x);
    int y1 = std::max(currect.y, rectangle.y);

    int x2 = std::min(currect.x + currect.w, rectangle.x + rectangle.w);
    int y2 = std::min(currect.y + currect.h, rectangle.y + rectangle.h);

    Rect newrect = { x1, y1, std::max(0, x2 - x1), std::max(0, y2 - y1) };
    SetScissor(newrect);
}
