#include "modules/graphics/Shader.hpp"
#include "driver/display/deko.hpp"

#include <cstring>

#define SHADERS_DIR "romfs:/shaders/"

#define DEFAULT_VERTEX_SHADER   (SHADERS_DIR "transform_vsh.dksh")
#define DEFAULT_FRAGMENT_SHADER (SHADERS_DIR "color_fsh.dksh")
#define DEFAULT_TEXTURE_SHADER  (SHADERS_DIR "texture_fsh.dksh")
#define DEFAULT_VIDEO_SHADER    (SHADERS_DIR "video_fsh.dksh")

namespace love
{
    Shader::Shader(StrongRef<ShaderStageBase> stages[SHADERSTAGE_MAX_ENUM], const CompileOptions& options) :
        ShaderBase(stages, options)
    {
        this->loadVolatile();
    }

    Shader::~Shader()
    {
        this->unloadVolatile();
    }

    void Shader::attach()
    {
        if (Shader::current != this)
        {
            Graphics::flushBatchedDrawsGlobal();

            d3d.useProgram(this->program.vertex, this->program.fragment);
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
        if (this->hasStage(ShaderStageType::SHADERSTAGE_VERTEX))
        {
            auto* stage = (dk::Shader*)this->stages[ShaderStageType::SHADERSTAGE_VERTEX]->getHandle();
            this->program.vertex = *stage;
        }

        if (this->hasStage(ShaderStageType::SHADERSTAGE_PIXEL))
        {
            auto* stage = (dk::Shader*)this->stages[ShaderStageType::SHADERSTAGE_PIXEL]->getHandle();
            this->program.fragment = *stage;
        }

        return true;
    }

    void Shader::unloadVolatile()
    {}

    void Shader::updateBuiltinUniforms(GraphicsBase* graphics)
    {}

    ptrdiff_t Shader::getHandle() const
    {
        return 0;
    }
} // namespace love
