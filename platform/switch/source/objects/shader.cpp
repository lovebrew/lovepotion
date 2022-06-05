#include "objects/shader/shader.h"

#include "common/bidirectionalmap.h"
#include "deko3d/deko.h"

using namespace love;

#define SHADERS_DIR "romfs:/shaders/"

#define DEFAULT_VERTEX_SHADER   (SHADERS_DIR "transform_vsh.dksh")
#define DEFAULT_FRAGMENT_SHADER (SHADERS_DIR "color_fsh.dksh")
#define DEFAULT_TEXTURE_SHADER  (SHADERS_DIR "texture_fsh.dksh")
#define DEFAULT_VIDEO_SHADER    (SHADERS_DIR "video_fsh.dksh")

Shader::Shader() : program()
{}

Shader::Shader(Data* vertex, Data* pixel) : program()
{
    std::string error;

    this->program.vertex->load(vertex->GetData(), vertex->GetSize());
    this->program.fragment->load(pixel->GetData(), pixel->GetSize());

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
    this->program.vertex->load(DEFAULT_VERTEX_SHADER);

    switch (type)
    {
        case STANDARD_DEFAULT:
            this->program.fragment->load(DEFAULT_FRAGMENT_SHADER);
            break;
        case STANDARD_TEXTURE:
            this->program.fragment->load(DEFAULT_TEXTURE_SHADER);
            break;
        case STANDARD_VIDEO:
            this->program.fragment->load(DEFAULT_VIDEO_SHADER);
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

const Shader::Program& Shader::GetProgram()
{
    return this->program;
}

void Shader::Attach()
{
    if (Shader::current != this)
    {
        ::deko3d::Instance().SetShader(this);
        Shader::current = this;
    }
}
