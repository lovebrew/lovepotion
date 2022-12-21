#include <objects/shader_ext.hpp>

#include <common/exception.hpp>

#include <citro3d.h>

using namespace love;

Shader<Console::CTR>::Shader()
{}

Shader<Console::CTR>::Shader(Data* shaderFile)
{}

Shader<Console::CTR>::~Shader()
{}

void Shader<Console::CTR>::LoadDefaults(StandardShader type)
{}

void Shader<Console::CTR>::AttachDefault(StandardShader type)
{}

void Shader<Console::CTR>::Attach()
{}

bool Shader<Console::CTR>::Validate(Data* data)
{
    return true;
}
