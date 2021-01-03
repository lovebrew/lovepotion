#include "deko3d/shader.h"

#include "deko3d/deko.h"

#include "s_vsh_dksh.h" //< Vertex Shader
#include "s_fsh_dksh.h" //< Default Fragment Shader
#include "t_fsh_dksh.h" //< Texture Fragment Shader

using namespace love;

Type love::Shader::type("Shader", &love::Object::type);

Shader * love::Shader::current = nullptr;
Shader * love::Shader::standardShaders[love::Shader::STANDARD_MAX_ENUM] = {nullptr};

Shader::Shader() : program()
{}

Shader::Shader(Data * vertex, Data * pixel) : program()
{
    std::string error;

    this->program.vertex->loadMemory(*dk3d.GetCode(), vertex->GetData(), vertex->GetSize());
    this->program.fragment->loadMemory(*dk3d.GetCode(), pixel->GetData(), pixel->GetSize());

    if (!this->Validate(*this->program.vertex, *this->program.fragment, error))
        throw love::Exception(error.c_str());
}

Shader::~Shader()
{
    for (int i = 0; i < STANDARD_MAX_ENUM; i++)
    {
        if (this == standardShaders[i])
            standardShaders[i] = nullptr;
    }

    if (current == this)
        Shader::AttachDefault(STANDARD_DEFAULT);
}

void Shader::LoadDefaults(StandardShader type)
{
    switch (type)
    {
        case STANDARD_DEFAULT:
            this->program.vertex->loadMemory(*dk3d.GetCode(),   (void *)s_vsh_dksh, s_vsh_dksh_size);
            this->program.fragment->loadMemory(*dk3d.GetCode(), (void *)s_fsh_dksh, s_fsh_dksh_size);
            break;
        case STANDARD_TEXTURE:
            this->program.vertex->loadMemory(*dk3d.GetCode(),   (void *)s_vsh_dksh, s_vsh_dksh_size);
            this->program.fragment->loadMemory(*dk3d.GetCode(), (void *)t_fsh_dksh, t_fsh_dksh_size);
            break;
        default:
            break;
    }

    std::string error;
    if (!this->Validate(*this->program.vertex, *this->program.fragment, error))
        throw love::Exception(error.c_str());
}

const char * Shader::GetStageName(CShader & shader)
{
    switch (shader.getStage())
    {
        case DkStage::DkStage_Vertex:
            return "Vertex";
        case DkStage::DkStage_Fragment:
            return "Fragment";
        default:
            break;
    }

    return NULL;
}

bool Shader::Validate(const CShader & vertex, const CShader & pixel, std::string & error)
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

void Shader::AttachDefault(StandardShader defaultType)
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

bool Shader::IsDefaultActive()
{
    for (int i = 0; i < STANDARD_MAX_ENUM; i++)
    {
        if (current == standardShaders[i])
            return true;
    }

    return false;
}

void Shader::Attach()
{
    if (Shader::current != this)
    {
        dk3d.UseProgram(this->program);

        Shader::current = this;
    }
}

bool Shader::GetConstant(const char * in, StandardShader & out)
{
    return shaderNames.Find(in, out);
}

bool Shader::GetConstant(StandardShader in, const char *& out)
{
    return shaderNames.Find(in, out);
}

StringMap<Shader::StandardShader, Shader::STANDARD_MAX_ENUM>::Entry Shader::shaderEntries[] =
{
    { "default", STANDARD_DEFAULT },
    { "texture", STANDARD_TEXTURE }
};

StringMap<Shader::StandardShader, Shader::STANDARD_MAX_ENUM> Shader::shaderNames(Shader::shaderEntries, sizeof(Shader::shaderEntries));