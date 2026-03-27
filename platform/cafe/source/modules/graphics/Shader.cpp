#include "common/Exception.hpp"
#include "common/config.hpp"
#include "common/screen.hpp"

#include "driver/display/GX2.hpp"
#include "driver/display/UniqueBuffer.hpp"

#include "modules/graphics/Shader.hpp"
#include "modules/graphics/ShaderStage.hpp"

#include <gfd.h>
#include <gx2/event.h>
#include <gx2/mem.h>
#include <gx2/utils.h>
#include <whb/gfx.h>

#include <malloc.h>
#include <span>

#include "common/debug.hpp"

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
        const auto vertexUniformBlocksCount = this->program.vertex->uniformBlockCount;
        for (size_t index = 0; index < vertexUniformBlocksCount; index++)
        {
            const auto uniform = this->program.vertex->uniformBlocks[index];

            UniformInfo info {};
            info.name      = uniform.name;
            info.location  = uniform.offset;
            info.stageMask = SHADERSTAGE_VERTEX;
            info.active    = true;
            info.count     = 1;

            this->reflection.localUniforms[info.name] = info;
        }

        const auto vertexUniformVariablesCount = this->program.vertex->uniformVarCount;
        for (size_t index = 0; index < vertexUniformVariablesCount; index++)
        {
            const auto uniform = this->program.vertex->uniformVars[index];

            UniformInfo info {};
            info.name      = uniform.name;
            info.location  = uniform.offset;
            info.stageMask = SHADERSTAGE_VERTEX;
            info.active    = true;
            info.count     = uniform.count;

            this->reflection.localUniforms[info.name] = info;
        }

        const auto pixelUniformVariablesCount = this->program.pixel->uniformVarCount;
        for (size_t index = 0; index < pixelUniformVariablesCount; index++)
        {
            const auto uniform = this->program.pixel->uniformVars[index];

            UniformInfo info {};
            info.name      = uniform.name;
            info.location  = uniform.offset;
            info.stageMask = SHADERSTAGE_PIXEL;
            info.active    = true;
            info.count     = uniform.count;

            this->reflection.localUniforms[info.name] = info;
        }

        const auto pixelSamplerVariablesCount = this->program.pixel->samplerVarCount;
        for (size_t index = 0; index < pixelSamplerVariablesCount; index++)
        {
            const auto sampler = this->program.pixel->samplerVars[index];

            UniformInfo info {};
            info.name      = sampler.name;
            info.location  = sampler.location;
            info.stageMask = SHADERSTAGE_PIXEL;
            info.active    = true;
            info.count     = 1;

            this->reflection.localUniforms[info.name] = info;
        }

        for (auto& kvp : this->reflection.localUniforms)
            reflection.uniforms[kvp.first] = &kvp.second;
    }

    bool Shader::loadVolatile()
    {
        for (const auto& stage : this->stages)
        {
            if (stage.get() != nullptr)
                ((ShaderStage*)stage.get())->loadVolatile();
        }

        if (this->hasStage(ShaderStageType::SHADERSTAGE_VERTEX))
            this->program.vertex = (GX2VertexShader*)stages[SHADERSTAGE_VERTEX]->getHandle();

        if (this->hasStage(ShaderStageType::SHADERSTAGE_PIXEL))
            this->program.pixel = (GX2PixelShader*)stages[SHADERSTAGE_PIXEL]->getHandle();

        if (!this->program.vertex || !this->program.pixel)
            return false;

        this->layout.reset();
        this->mapActiveUniforms();

        this->transformation.flags     = GX2_BUFFERFLAG_UNIFORM_BLOCK;
        this->transformation.elemSize  = sizeof(float) * 4 * 4 * 2;
        this->transformation.elemCount = 1;

        return GX2RCreateBuffer(&this->transformation);
    }

    void Shader::unloadVolatile()
    {
        WHBGfxFreeVertexShader(this->program.vertex);
        WHBGfxFreePixelShader(this->program.pixel);
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

    static void bswap_into(Matrix4& out, const Matrix4& in)
    {
        auto src = std::span<const uint32_t>(reinterpret_cast<const uint32_t*>(&in), 16);
        auto dst = std::span<uint32_t>(reinterpret_cast<uint32_t*>(&out), 16);

        for (size_t index = 0; index < src.size(); index++)
            dst[index] = std::byteswap(src[index]);
    }

    void Shader::updateBuiltinUniforms(GraphicsBase* graphics)
    {
        if (current != this)
            return;

        BuiltinUniformData data {};
        bswap_into(data.transformMatrix, graphics->getTransform());
        bswap_into(data.projectionMatrix, graphics->getDeviceProjection());

        auto* transformation = this->getUniformInfo("Transformation");

        if (transformation == nullptr)
            return;

        UniqueBuffer buffer(&this->transformation);
        buffer.write(0, sizeof(BuiltinUniformData), &data);

        /* this is actually (GX2RBuffer*, offset, binding)*/
        GX2RSetVertexUniformBlock(&this->transformation, transformation->location, 0);
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

            current = this;
            shaderSwitches++;

            GX2SetVertexShader(this->program.vertex);
            GX2SetPixelShader(this->program.pixel);
        }
    }
} // namespace love
