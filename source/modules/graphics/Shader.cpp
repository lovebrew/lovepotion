#include "modules/graphics/Shader.tcc"

namespace love
{
    ShaderBase* ShaderBase::current                            = nullptr;
    ShaderBase* ShaderBase::standardShaders[STANDARD_MAX_ENUM] = { nullptr };

    int ShaderBase::shaderSwitches = 0;

    ShaderBase::~ShaderBase()
    {
        for (int index = 0; index < STANDARD_MAX_ENUM; index++)
        {
            if (this == standardShaders[index])
                standardShaders[index] = nullptr;
        }

        if (current == this)
            this->attachDefault(STANDARD_DEFAULT);
    }

    void ShaderBase::attachDefault(StandardShader type)
    {
        auto* defaultShader = standardShaders[type];

        if (defaultShader == nullptr)
        {
            current = nullptr;
            return;
        }

        if (current != defaultShader)
            defaultShader->attach();
    }

    bool ShaderBase::isDefaultActive()
    {
        for (int index = 0; index < STANDARD_MAX_ENUM; index++)
        {
            if (current == standardShaders[index])
                return true;
        }

        return false;
    }
} // namespace love
