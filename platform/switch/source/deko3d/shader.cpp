#include "common/runtime.h"
#include "deko3d/shader.h"


using namespace love;

love::Type love::Shader::type("Shader", &love::Object::type);

love::Shader * love::Shader::current = nullptr;
love::Shader * love::Shader::standardShaders[love::Shader::STANDARD_MAX_ENUM] = {nullptr};

love::Shader::Shader(ShaderStage * vertex, ShaderStage * pixel) : stages()
{
    std::string error;
    if (!this->Validate(vertex, pixel, error))
        throw love::Exception("%s", error.c_str());

    this->stages[ShaderStage::STAGE_VERTEX] = vertex;
    this->stages[ShaderStage::STAGE_PIXEL] = pixel;

    if (current == this)
    {
        // make sure glUseProgram gets called.
        current = nullptr;
        this->Attach();
        // updateBuiltinUniforms();
    }
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

bool love::Shader::Validate(ShaderStage * vertex, ShaderStage * pixel, std::string & error)
{
    if (vertex != nullptr && !vertex->GetShader().isValid())
    {
        error = "Invalid vertex shader.";
        return false;
    }

    if (pixel != nullptr && !pixel->GetShader().isValid())
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
    if (current != this)
    {
        // Graphics::FlushStreamDrawsGlobal();
        dk3d.UseProgram({this->stages[ShaderStage::STAGE_VERTEX]->GetShader(),
                         this->stages[ShaderStage::STAGE_PIXEL]->GetShader()});

        current = this;
    }
}