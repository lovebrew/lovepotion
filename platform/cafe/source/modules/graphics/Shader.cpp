#include "common/Exception.hpp"
#include "common/screen.hpp"

#include "modules/graphics/Shader.hpp"
#include "modules/graphics/ShaderStage.hpp"

#include <gfd.h>
#include <gx2/mem.h>
#include <whb/gfx.h>

#include <malloc.h>

#define SHADERS_DIR "/vol/content/shaders/"

#define DEFAULT_PRIMITIVE_SHADER (SHADERS_DIR "color.gsh")
#define DEFAULT_TEXTURE_SHADER   (SHADERS_DIR "texture.gsh")
#define DEFAULT_VIDEO_SHADER     (SHADERS_DIR "video.gsh")

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

        switch (shader)
        {
            case STANDARD_DEFAULT:
            default:
                return DEFAULT_PRIMITIVE_SHADER;
            case STANDARD_TEXTURE:
                return DEFAULT_TEXTURE_SHADER;
            case STANDARD_VIDEO:
                return DEFAULT_VIDEO_SHADER;
        }
    }

    void Shader::mapActiveUniforms()
    {
        const auto uniformBlockCount = this->program.vertexShader->uniformBlockCount;

        for (size_t index = 0; index < uniformBlockCount; index++)
        {
            const auto uniform = this->program.vertexShader->uniformBlocks[index];

            this->reflection.uniforms.insert_or_assign(
                uniform.name, new UniformInfo {
                                  .type      = UNIFORM_MATRIX,
                                  .stageMask = ShaderStageMask::SHADERSTAGEMASK_VERTEX,
                                  .active    = true,
                                  .location  = uniform.offset,
                                  .count     = 1,
                                  .name      = uniform.name,
                              });
        }

        const auto samplerCount = this->program.pixelShader->samplerVarCount;

        for (size_t index = 0; index < samplerCount; index++)
        {
            const auto sampler = this->program.pixelShader->samplerVars[index];

            this->reflection.uniforms.insert_or_assign(
                sampler.name, new UniformInfo {
                                  .type      = UNIFORM_SAMPLER,
                                  .stageMask = ShaderStageMask::SHADERSTAGEMASK_PIXEL,
                                  .active    = true,
                                  .location  = sampler.location,
                                  .count     = 1,
                                  .name      = sampler.name,
                              });
        }
    }

    bool Shader::setShaderStages(WHBGfxShaderGroup* group, std::array<StrongRef<ShaderStageBase>, 2> stages)
    {
        std::memset(group, 0, sizeof(WHBGfxShaderGroup));

        if (this->hasStage(ShaderStageType::SHADERSTAGE_VERTEX))
            group->vertexShader = (GX2VertexShader*)stages[SHADERSTAGE_VERTEX]->getHandle();

        if (this->hasStage(ShaderStageType::SHADERSTAGE_PIXEL))
            group->pixelShader = (GX2PixelShader*)stages[SHADERSTAGE_PIXEL]->getHandle();

        if (!this->program.vertexShader || !this->program.pixelShader)
            return false;

        return true;
    }

    bool Shader::loadVolatile()
    {
        for (const auto& stage : this->stages)
        {
            if (stage.get() != nullptr)
                ((ShaderStage*)stage.get())->loadVolatile();
        }

        if (!this->setShaderStages(&this->program, this->stages))
            return true;

        this->mapActiveUniforms();

        // clang-format off
        WHBGfxInitShaderAttribute(&this->program, "inPos",      0, POSITION_OFFSET, GX2_ATTRIB_FORMAT_FLOAT_32_32);
        WHBGfxInitShaderAttribute(&this->program, "inTexCoord", 0, TEXCOORD_OFFSET, GX2_ATTRIB_FORMAT_FLOAT_32_32);
        WHBGfxInitShaderAttribute(&this->program, "inColor",    0, COLOR_OFFSET,    GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32);
        // clang-format on

        if (!WHBGfxInitFetchShader(&this->program))
            return false;

        return true;
    }

    void Shader::unloadVolatile()
    {
        WHBGfxFreeShaderGroup(&this->program);

        for (auto& it : this->reflection.uniforms)
            delete it.second;
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
            if (!_warnings.empty() && ShaderStage::getConstant(stage->getStageType(), stageString))
                warnings += std::format("{} shader:\n{}", stageString, _warnings);
        }

        return warnings;
    }

    void Shader::updateBuiltinUniforms(GraphicsBase* graphics, Uniform* uniform)
    {
        if (current != this)
            return;

        auto& transform = graphics->getTransform();
        // uniform->update(transform);

        auto* uniformBlock = this->getUniformInfo("Transformation");

        if (!uniformBlock)
            return;

        GX2Invalidate(INVALIDATE_UNIFORM_BLOCK, uniform, UNIFORM_SIZE);
        GX2SetVertexUniformBlock(uniformBlock->location, UNIFORM_SIZE, uniform);
    }

    ptrdiff_t Shader::getHandle() const
    {
        return 0;
    }

    void Shader::attach()
    {
        if (current != this)
        {
            Graphics::flushBatchedDrawsGlobal();

            GX2SetShaderMode(GX2_SHADER_MODE_UNIFORM_BLOCK);

            GX2SetFetchShader(&this->program.fetchShader);
            GX2SetVertexShader(this->program.vertexShader);
            GX2SetPixelShader(this->program.pixelShader);

            current = this;
            shaderSwitches++;
        }
    }
} // namespace love
