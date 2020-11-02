#include "common/runtime.h"
#include "deko3d/shader.h"

#include "deko3d/deko.h"

#include "s_vsh_dksh.h" // Vertex Shader
#include "s_fsh_dksh.h" // Default Fragment Shader
#include "t_fsh_dksh.h" // Texture Fragment Shader

using namespace love;

love::Type love::Shader::type("Shader", &love::Object::type);

love::Shader * love::Shader::current = nullptr;
love::Shader * love::Shader::standardShaders[love::Shader::STANDARD_MAX_ENUM] = {nullptr};

love::Shader::Shader() : program()
{}

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

void love::Shader::LoadDefaults(StandardShader type)
{
    switch (type)
    {
        case STANDARD_DEFAULT:
            this->program.vertex.loadMemory(*dk3d.GetCode(), (void *)s_vsh_dksh, s_vsh_dksh_size);
            this->program.fragment.loadMemory(*dk3d.GetCode(), (void *)s_fsh_dksh, s_fsh_dksh_size);
            break;
        case STANDARD_TEXTURE:
        default:
            this->program.vertex.loadMemory(*dk3d.GetCode(), (void *)s_vsh_dksh, s_vsh_dksh_size);
            this->program.fragment.loadMemory(*dk3d.GetCode(), (void *)t_fsh_dksh, t_fsh_dksh_size);
            break;
    }
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