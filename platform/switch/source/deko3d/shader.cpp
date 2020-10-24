#include "common/runtime.h"
#include "deko3d/shader.h"


using namespace love;

love::Type love::Shader::type("Shader", &love::Object::type);

love::Shader * love::Shader::current = nullptr;
love::Shader * love::Shader::standardShaders[love::Shader::STANDARD_MAX_ENUM] = {nullptr};

love::Shader::Shader(ShaderStage * vertex, ShaderStage * pixel) : stages()
{
    this->stages[ShaderStage::STAGE_VERTEX] = vertex;
    this->stages[ShaderStage::STAGE_PIXEL] = pixel;
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
        dk3d.UseProgram({this->vertexShader, this->fragmentShader});
        current = this;
    }
}