#include "objects/shader/shaderc.h"

love::Type love::common::Shader::type("Shader", &love::Object::type);

using namespace love::common;

Shader* Shader::current                                    = nullptr;
Shader* Shader::standardShaders[Shader::STANDARD_MAX_ENUM] = { nullptr };

Shader::Shader()
{}

Shader::Shader(love::Data* vertex, love::Data* pixel)
{}

Shader::~Shader()
{}

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
