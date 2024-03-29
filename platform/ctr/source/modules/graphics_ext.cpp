#include <modules/fontmodule_ext.hpp>
#include <modules/graphics_ext.hpp>

#include <modules/window_ext.hpp>

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

void Graphics<Console::CTR>::SetShader()
{
    Shader<Console::CTR>::AttachDefault(Shader<>::STANDARD_DEFAULT);
    this->states.back().shader.Set(nullptr);
}

void Graphics<Console::CTR>::SetShader(Shader<Console::CTR>* shader)
{
    if (shader == nullptr)
        return this->SetShader();

    shader->Attach();
    this->states.back().shader.Set(shader);
}

void Graphics<Console::CTR>::Draw(Drawable* drawable, const Matrix4& matrix)
{
    drawable->Draw(*this, matrix);
}

void Graphics<Console::CTR>::Draw(Texture<Console::CTR>* texture, Quad* quad,
                                  const Matrix4& matrix)
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

    const auto type = Shader<>::STANDARD_DEFAULT;

    try
    {
        if (!Shader<Console::CTR>::defaults[type])
        {
            auto* shader = new Shader<Console::CTR>();
            shader->LoadDefaults(type);

            Shader<Console::CTR>::defaults[type] = shader;
        }
    }
    catch (love::Exception&)
    {
        throw;
    }

    if (!Shader<Console::CTR>::current)
        Shader<Console::CTR>::defaults[Shader<>::STANDARD_DEFAULT]->Attach();

    this->created = true;
}
