#include "common/runtime.h"
#include "deko3d/shader.h"

#include "deko3d/deko.h"

using namespace love;

love::Type love::Shader::type("Shader", &love::Object::type);

love::Shader * love::Shader::current = nullptr;
love::Shader * love::Shader::standardShaders[love::Shader::STANDARD_MAX_ENUM] = {nullptr};

love::Shader::Shader(CShader && vertex, CShader && pixel)
{
    std::string error;
    if (!this->Validate(vertex, pixel, error))
        throw love::Exception("%s", error.c_str());

    this->program =
    {
        .vertex   = std::move(vertex),
        .fragment = std::move(pixel)
    };
}

love::Shader::~Shader()
{
    for (int i = 0; i < STANDARD_MAX_ENUM; i++)
    {
        if (this == standardShaders[i])
            standardShaders[i] = nullptr;
    }

    if (current == this)
        Shader::AttachDefault(STANDARD_DEFAULT);
}

bool love::Shader::Validate(const CShader & vertex, const CShader & pixel, std::string & error)
{
    if (!vertex.isValid())
    {
        error = "Invalid vertex shader.";
        return false;
    }

    if (!pixel.isValid())
    {
        error = "Invalid fragment shader.";
        return false;
    }

    return true;
}

void love::Shader::AttachDefault(StandardShader defaultType)
{
    Shader * defaultshader = standardShaders[defaultType];

    if (defaultshader == nullptr)
    {
        current = nullptr;
        return;
    }

    if (current != defaultshader)
        defaultshader->Attach();
}

bool love::Shader::IsDefaultActive()
{
    for (int i = 0; i < STANDARD_MAX_ENUM; i++)
    {
        if (current == standardShaders[i])
            return true;
    }

    return false;
}

void love::Shader::Attach()
{
    if (Shader::current != this)
    {
        dk3d.UseProgram(this->program);

        Shader::current = this;
    }
}