#include <modules/graphics_ext.hpp>

#include <modules/window_ext.hpp>

#include <modules/fontmodule_ext.hpp>

#include <objects/texture_ext.hpp>

#include <objects/shader_ext.hpp>

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

void Graphics<Console::HAC>::SetShader()
{
    Shader<Console::HAC>::AttachDefault(Shader<>::STANDARD_DEFAULT);
    this->states.back().shader.Set(nullptr);
}

void Graphics<Console::HAC>::SetShader(Shader<Console::HAC>* shader)
{
    if (shader == nullptr)
        return this->SetShader();

    shader->Attach();
    this->states.back().shader.Set(shader);
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

Texture<Console::HAC>* Graphics<Console::HAC>::NewTexture(const Texture<>::Settings& settings,
                                                          const Texture<>::Slices* slices) const
{
    return new Texture<Console::HAC>(this, settings, slices);
}

void Graphics<Console::HAC>::Draw(Drawable* drawable, const Matrix4& matrix)
{
    drawable->Draw(*this, matrix);
}

void Graphics<Console::HAC>::Draw(Texture<Console::HAC>* texture, Quad* quad,
                                  const Matrix4& matrix)
{
    texture->Draw(*this, quad, matrix);
}

void Graphics<Console::HAC>::SetViewportSize(int width, int height)
{
    ::Renderer::Instance().SetViewport({ 0, 0, width, height });
    this->SetScissor({ 0, 0, width, height });
}
