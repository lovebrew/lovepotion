#include "objects/shader/shaderc.h"

#include "common/bidirectionalmap.h"

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

// clang-format off
constexpr auto shaderNames = BidirectionalMap<>::Create(
    "default", Shader::StandardShader::STANDARD_DEFAULT,
    "texture", Shader::StandardShader::STANDARD_TEXTURE,
    "video",   Shader::StandardShader::STANDARD_VIDEO
);
// clang-format on

bool Shader::GetConstant(const char* in, StandardShader& out)
{
    return shaderNames.Find(in, out);
}

bool Shader::GetConstant(StandardShader in, const char*& out)
{
    return shaderNames.ReverseFind(in, out);
}
