#include "common/Exception.hpp"

#include "driver/display/citro3d.hpp"
#include "modules/graphics/Shader.hpp"

#include "common/config.hpp"

#define SHADERS_DIR "romfs:/shaders/"

#define DEFAULT_SHADER (SHADERS_DIR "main_v_pica.shbin")

namespace love
{
    Shader::Shader(StrongRef<ShaderStageBase> _stages[SHADERSTAGE_MAX_ENUM], const CompileOptions& options) :
        ShaderBase(_stages, options)
    {
        this->loadVolatile();
    }

    Shader::~Shader()
    {
        this->unloadVolatile();
    }

    const char* Shader::getDefaultStagePath(StandardShader shader, ShaderStageType stage)
    {
        LOVE_UNUSED(stage);
        return DEFAULT_SHADER;
    }

    void Shader::mapActiveUniforms()
    {
        std::vector names { "mdlvMtx", "projMtx" };

        for (const auto* name : names)
        {
            int8_t location = shaderInstanceGetUniformLocation(this->program.vertexShader, name);

            if (location < 0)
                throw love::Exception("Failed to get uniform location for {:s}", name);

            this->reflection.uniforms[name] =
                new UniformInfo { .type      = UNIFORM_MATRIX,
                                  .stageMask = ShaderStageMask::SHADERSTAGEMASK_VERTEX,
                                  .active    = true,
                                  .location  = location,
                                  .count     = 1,
                                  .name      = name };
        }
    }

    bool Shader::loadVolatile()
    {
        if (this->hasStage(ShaderStageType::SHADERSTAGE_VERTEX))
        {
            const auto* dvlb = (DVLB_s*)this->stages[SHADERSTAGE_VERTEX]->getHandle();

            if (R_FAILED(shaderProgramInit(&this->program)))
                throw love::Exception("Failed to load shader program.");

            if (R_FAILED(shaderProgramSetVsh(&this->program, &dvlb->DVLE[0])))
                throw love::Exception("Failed to set shader vertex shader.");

            this->mapActiveUniforms();

            return true;
        }

        return false;
    }

    void Shader::unloadVolatile()
    {
        shaderProgramFree(&this->program);
    }

    static void updateTransform(C3D_Mtx& matrix, const Matrix4& transform)
    {
        for (int row = 0; row < 4; row++)
        {
            for (int column = 0; column < 4; column++)
                matrix.m[row * 4 + (3 - column)] = transform.get(row, column);
        }
    }

    void Shader::updateBuiltinUniforms(GraphicsBase* graphics, C3D_Mtx mdlvMtx, const C3D_Mtx& projMtx)
    {
        if (this->hasUniform("mdlvMtx"))
        {
            updateTransform(mdlvMtx, graphics->getTransform());
            C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, this->reflection.uniforms["mdlvMtx"]->location, &mdlvMtx);
        }

        if (this->hasUniform("projMtx"))
            C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, this->reflection.uniforms["projMtx"]->location, &projMtx);
    }

    void Shader::attach()
    {
        if (current != this)
        {
            Graphics::flushBatchedDrawsGlobal();

            C3D_BindProgram(&this->program);
            current = this;
        }
    }

    ptrdiff_t Shader::getHandle() const
    {
        return 0;
    }
} // namespace love
