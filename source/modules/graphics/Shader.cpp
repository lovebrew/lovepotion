#include "modules/graphics/Shader.tcc"

namespace love
{
    ShaderBase* ShaderBase::current                            = nullptr;
    ShaderBase* ShaderBase::standardShaders[STANDARD_MAX_ENUM] = { nullptr };
} // namespace love
