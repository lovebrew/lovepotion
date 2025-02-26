#include "common/Exception.hpp"

#include "modules/graphics/Shader.tcc"

namespace love
{
    ShaderBase* ShaderBase::current                            = nullptr;
    ShaderBase* ShaderBase::standardShaders[STANDARD_MAX_ENUM] = { nullptr };

    int ShaderBase::shaderSwitches = 0;

    ShaderBase::ShaderBase(StrongRef<ShaderStageBase> _stages[], const CompileOptions& options) :
        stages(),
        debugName(options.debugName)
    {
        for (int i = 0; i < SHADERSTAGE_MAX_ENUM; i++)
            this->stages[i] = _stages[i];
    }

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

    bool ShaderBase::hasStage(ShaderStageType stage)
    {
        return this->stages[stage] != nullptr;
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

    const ShaderBase::UniformInfo* ShaderBase::getUniformInfo(const std::string& name) const
    {
        const auto it = this->reflection.uniforms.find(name);
        return it != this->reflection.uniforms.end() ? it->second : nullptr;
    }

    bool ShaderBase::hasUniform(const std::string& name) const
    {
        const auto it = this->reflection.uniforms.find(name);
        return it != this->reflection.uniforms.end() && it->second->active;
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
