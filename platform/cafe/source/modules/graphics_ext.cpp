#include <modules/graphics_ext.hpp>

#include <utilities/driver/renderer_ext.hpp>

#include <common/matrix_ext.hpp>

#include <modules/fontmodule_ext.hpp>
#include <modules/window_ext.hpp>

#include <objects/texture_ext.hpp>

#include <objects/shader_ext.hpp>

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

void Graphics<Console::CAFE>::SetViewportSize(int width, int height)
{
    ::Renderer::Instance().SetViewport(Rect::EMPTY);
    this->SetScissor(Rect::EMPTY);
}
