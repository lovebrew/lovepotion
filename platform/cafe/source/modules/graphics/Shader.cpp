#include "common/Exception.hpp"
#include "common/config.hpp"
#include "common/screen.hpp"

#include "modules/graphics/Shader.hpp"
#include "modules/graphics/ShaderStage.hpp"

#include <gfd.h>
#include <gx2/event.h>
#include <gx2/mem.h>
#include <gx2/utils.h>
#include <whb/gfx.h>

#include <malloc.h>

#include "common/debug.hpp"

#define SHADERS_DIR "/vol/content/shaders/"

#define DEFAULT_PRIMITIVE_SHADER (SHADERS_DIR "color.gsh")
#define DEFAULT_TEXTURE_SHADER   (SHADERS_DIR "texture.gsh")
#define DEFAULT_VIDEO_SHADER     (SHADERS_DIR "video.gsh")

namespace love
{
    Shader::Shader(StrongRef<ShaderStageBase> _stages[SHADERSTAGE_MAX_ENUM], const CompileOptions& options) :
        ShaderBase(_stages, options),
        attributesDirty(true),
        attributes {},
        shader {}
    {
        this->loadVolatile();
        this->attributes.resize(16);
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
            LOG("UniformBlock: %s (location %u)", uniform.name, uniform.offset);
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
            LOG("VtxUniform: %s (location %u)", uniform.name, uniform.offset);
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
            LOG("PixUniform: %s (location %u)", uniform.name, uniform.offset);
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

        this->mapActiveUniforms();

        return true;
    }

    static uint32_t getVertexAttributeLocation(const GX2VertexShader* shader, const char* name)
    {
        for (uint32_t i = 0; i < shader->attribVarCount; ++i)
        {
            if (strcmp(shader->attribVars[i].name, name) == 0)
                return shader->attribVars[i].location;
        }

        return -1;
    }

    static uint32_t getAttributeFormatSelector(GX2AttribFormat format)
    {
        switch (format)
        {
            case GX2_ATTRIB_FORMAT_UNORM_8:
            case GX2_ATTRIB_FORMAT_UINT_8:
            case GX2_ATTRIB_FORMAT_SNORM_8:
            case GX2_ATTRIB_FORMAT_SINT_8:
            case GX2_ATTRIB_FORMAT_FLOAT_32:
                return GX2_SEL_MASK(GX2_SQ_SEL_X, GX2_SQ_SEL_0, GX2_SQ_SEL_0, GX2_SQ_SEL_1);
            case GX2_ATTRIB_FORMAT_UNORM_8_8:
            case GX2_ATTRIB_FORMAT_UINT_8_8:
            case GX2_ATTRIB_FORMAT_SNORM_8_8:
            case GX2_ATTRIB_FORMAT_SINT_8_8:
            case GX2_ATTRIB_FORMAT_FLOAT_32_32:
                return GX2_SEL_MASK(GX2_SQ_SEL_X, GX2_SQ_SEL_Y, GX2_SQ_SEL_0, GX2_SQ_SEL_1);
            case GX2_ATTRIB_FORMAT_FLOAT_32_32_32:
                return GX2_SEL_MASK(GX2_SQ_SEL_X, GX2_SQ_SEL_Y, GX2_SQ_SEL_Z, GX2_SQ_SEL_1);
            case GX2_ATTRIB_FORMAT_UNORM_8_8_8_8:
            case GX2_ATTRIB_FORMAT_UINT_8_8_8_8:
            case GX2_ATTRIB_FORMAT_SNORM_8_8_8_8:
            case GX2_ATTRIB_FORMAT_SINT_8_8_8_8:
            case GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32:
                return GX2_SEL_MASK(GX2_SQ_SEL_X, GX2_SQ_SEL_Y, GX2_SQ_SEL_Z, GX2_SQ_SEL_W);
                break;
            default:
                return GX2_SEL_MASK(GX2_SQ_SEL_0, GX2_SQ_SEL_0, GX2_SQ_SEL_0, GX2_SQ_SEL_1);
        }
    }

    bool Shader::initAttribute(uint32_t bufferIndex, uint32_t offset, uint32_t index, GX2AttribFormat format)
    {
        const char* name = love::getConstant((BuiltinVertexAttribute)index);
        if (name == nullptr)
            return false;

        int32_t location = getVertexAttributeLocation(this->program.vertex, name);

        if (location < 0)
            return false;

        GX2AttribStream& attribute = this->attributes[index];
        attribute.location         = location;
        attribute.buffer           = bufferIndex;
        attribute.offset           = offset;
        attribute.format           = format;
        attribute.type             = GX2_ATTRIB_INDEX_PER_VERTEX;
        attribute.aluDivisor       = 0;
        attribute.mask             = getAttributeFormatSelector(format);
        attribute.endianSwap       = GX2_ENDIAN_SWAP_DEFAULT;

        this->attributesDirty = true;
        return true;
    }

    bool Shader::initFetchShader(size_t enabledBits)
    {
        if (!this->attributesDirty)
            return true;

        if (this->attributes.empty())
            return false;

        std::vector<GX2AttribStream> attribs {};
        for (size_t index = 0; index < this->attributes.size(); index++)
        {
            if (enabledBits & (1u << index))
                attribs.push_back(this->attributes[index]);
        }
        const auto count = attribs.size();

        if (this->shader.program)
            std::free(this->shader.program);

        const auto size = GX2CalcFetchShaderSizeEx(count, TESSELATION_NONE, TESSELATION_DISCRETE);

        this->shader.size    = size;
        this->shader.program = memalign(GX2_SHADER_PROGRAM_ALIGNMENT, size);

        if (this->shader.program == nullptr)
            throw love::Exception(E_OUT_OF_MEMORY);

        // clang-format off
        GX2InitFetchShaderEx(&this->shader, (uint8_t*)this->shader.program, count, attribs.data(), TESSELATION_NONE, TESSELATION_DISCRETE);
        GX2Invalidate(GX2_INVALIDATE_MODE_CPU_SHADER, this->shader.program, size);
        this->attributesDirty = false;
        // clang-format on

        GX2SetShaderMode(GX2_SHADER_MODE_UNIFORM_BLOCK);
        GX2SetFetchShader(&this->shader);

        return true;
    }

    void Shader::unloadVolatile()
    {
        WHBGfxFreeVertexShader(this->program.vertex);
        WHBGfxFreePixelShader(this->program.pixel);

        if (this->shader.program)
            std::free(this->shader.program);
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
        uniform->update(transform);

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

            GX2SetVertexShader(this->program.vertex);
            GX2SetPixelShader(this->program.pixel);

            current = this;
            shaderSwitches++;
        }
    }
} // namespace love
