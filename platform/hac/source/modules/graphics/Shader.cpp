#include "common/Exception.hpp"

#include "driver/display/deko.hpp"
#include "modules/graphics/Shader.hpp"
#include "modules/graphics/ShaderStage.hpp"

#include <cstring>

#define SHADERS_DIR "romfs:/shaders/"

#define DEFAULT_VERTEX_SHADER   (SHADERS_DIR "transform_vsh.dksh")
#define DEFAULT_FRAGMENT_SHADER (SHADERS_DIR "color_fsh.dksh")
#define DEFAULT_TEXTURE_SHADER  (SHADERS_DIR "texture_fsh.dksh")
#define DEFAULT_VIDEO_SHADER    (SHADERS_DIR "video_fsh.dksh")

namespace love
{
    Shader::Shader(StrongRef<ShaderStageBase> stages[SHADERSTAGE_MAX_ENUM], const CompileOptions& options) :
        ShaderBase(stages, options),
        program { nullptr, nullptr }
    {
        this->loadVolatile();
    }

    Shader::~Shader()
    {
        this->unloadVolatile();
    }

    std::string Shader::getWarnings() const
    {
        std::string warnings {};
        std::string_view stageString;

        for (const auto& stage : this->stages)
        {
            if (stage.get() == nullptr)
                continue;

            const std::string& _warnings = stage->getWarnings();
            if (!_warnings.empty() && ShaderStageBase::getConstant(stage->getStageType(), stageString))
                warnings += std::format("{} shader:\n{}", stageString, _warnings);
        }

        return warnings;
    }

    void Shader::attach()
    {
        if (Shader::current != this)
        {
            Graphics::flushBatchedDrawsGlobal();

            d3d.useProgram({ this->program.vertex, this->program.fragment });
            ++shaderSwitches;

            Shader::current = this;
        }
    }

    const char* Shader::getDefaultStagePath(StandardShader shader, ShaderStageType stage)
    {
        if (stage == SHADERSTAGE_VERTEX)
            return DEFAULT_VERTEX_SHADER;

        switch (shader)
        {
            case STANDARD_DEFAULT:
            default:
                return DEFAULT_FRAGMENT_SHADER;
            case STANDARD_TEXTURE:
                return DEFAULT_TEXTURE_SHADER;
            case STANDARD_VIDEO:
                return DEFAULT_VIDEO_SHADER;
        }
    }

    void Shader::mapActiveUniforms()
    {}

    bool Shader::loadVolatile()
    {
        for (auto stage : this->stages)
        {
            if (stage.get() != nullptr)
                ((ShaderStage*)stage.get())->loadVolatile();
        }

        if (this->hasStage(ShaderStageType::SHADERSTAGE_VERTEX))
        {
            this->program.vertex =
                (dk::Shader*)this->stages[ShaderStageType::SHADERSTAGE_VERTEX]->getHandle();
        }

        if (this->hasStage(ShaderStageType::SHADERSTAGE_PIXEL))
        {
            this->program.fragment =
                (dk::Shader*)this->stages[ShaderStageType::SHADERSTAGE_PIXEL]->getHandle();
        }

        return true;
    }

    void Shader::unloadVolatile()
    {
        this->program.vertex   = nullptr;
        this->program.fragment = nullptr;
    }

    void Shader::updateBuiltinUniforms(GraphicsBase* graphics, glm::mat4& model)
    {}

    ptrdiff_t Shader::getHandle() const
    {
        return 0;
    }
} // namespace love
