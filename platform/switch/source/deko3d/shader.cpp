#include "deko3d/shader.h"

#include "deko3d/deko.h"

using namespace love;

Type love::Shader::type("Shader", &love::Object::type);

Shader* love::Shader::current                                          = nullptr;
Shader* love::Shader::standardShaders[love::Shader::STANDARD_MAX_ENUM] = { nullptr };

#define SHADERS_DIR "romfs:/shaders/"

#define DEFAULT_VERTEX_SHADER   (SHADERS_DIR "transform_vsh.dksh")
#define DEFAULT_FRAGMENT_SHADER (SHADERS_DIR "color_fsh.dksh")
#define DEFAULT_TEXTURE_SHADER  (SHADERS_DIR "texture_fsh.dksh")

Shader::Shader() : program()
{}

Shader::Shader(Data* vertex, Data* pixel) : program()
{
    std::string error;

    this->program.vertex->load(::deko3d::Instance().GetCode(), vertex->GetData(),
                               vertex->GetSize());
    this->program.fragment->load(::deko3d::Instance().GetCode(), pixel->GetData(),
                                 pixel->GetSize());

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
            this->program.vertex->load(::deko3d::Instance().GetCode(), DEFAULT_VERTEX_SHADER);
            this->program.fragment->load(::deko3d::Instance().GetCode(), DEFAULT_FRAGMENT_SHADER);
            break;
        case STANDARD_TEXTURE:
            this->program.vertex->load(::deko3d::Instance().GetCode(), DEFAULT_VERTEX_SHADER);
            this->program.fragment->load(::deko3d::Instance().GetCode(), DEFAULT_TEXTURE_SHADER);
            break;
        default:
            break;
    }

    std::string error;
    if (!this->Validate(*this->program.vertex, *this->program.fragment, error))
        throw love::Exception(error.c_str());
}

const char* Shader::GetStageName(CShader& shader)
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

bool Shader::Validate(const CShader& vertex, const CShader& pixel, std::string& error)
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
    Shader* defaultshader = standardShaders[defaultType];

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
        ::deko3d::Instance().UseProgram(this->program);

        Shader::current = this;
    }
}

bool Shader::GetConstant(const char* in, StandardShader& out)
{
    return shaderNames.Find(in, out);
}

bool Shader::GetConstant(StandardShader in, const char*& out)
{
    return shaderNames.Find(in, out);
}

// clang-format off
constexpr StringMap<Shader::StandardShader, Shader::STANDARD_MAX_ENUM>::Entry shaderEntries[] =
{
    { "default", Shader::StandardShader::STANDARD_DEFAULT },
    { "texture", Shader::StandardShader::STANDARD_TEXTURE }
};

constinit const StringMap<Shader::StandardShader, Shader::STANDARD_MAX_ENUM> Shader::shaderNames(shaderEntries);
// clang-format on
