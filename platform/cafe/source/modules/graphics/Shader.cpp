#include "modules/graphics/Shader.hpp"

#include "common/screen.hpp"

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
    Shader::Shader(StandardShader type) : ShaderBase(type), uniform {}
    {
        std::string error;

        switch (type)
        {
            default:
            case STANDARD_DEFAULT:
            {
                if (!this->validate(DEFAULT_PRIMITIVE_SHADER, error))
                    throw love::Exception("Failed to load primitive shader: {:s}", error);

                break;
            }
            case STANDARD_TEXTURE:
            {
                if (!this->validate(DEFAULT_TEXTURE_SHADER, error))
                    throw love::Exception("Failed to load texture shader: {:s}", error);

                break;
            }
            case STANDARD_VIDEO:
            {
                if (!this->validate(DEFAULT_VIDEO_SHADER, error))
                    throw love::Exception("Failed to load video shader: {:s}", error);

                break;
            }
        }

        // clang-format off
        WHBGfxInitShaderAttribute(&this->program, "inPos",      0, POSITION_OFFSET, GX2_ATTRIB_FORMAT_FLOAT_32_32);
        WHBGfxInitShaderAttribute(&this->program, "inTexCoord", 0, TEXCOORD_OFFSET, GX2_ATTRIB_FORMAT_FLOAT_32_32);
        WHBGfxInitShaderAttribute(&this->program, "inColor",    0, COLOR_OFFSET,    GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32);
        // clang-format on

        this->uniform = this->getUniform("Transformation");

        if (!WHBGfxInitFetchShader(&this->program))
            throw love::Exception("Failed to initialize Fetch Shader");
    }

    Shader::~Shader()
    {
        this->unloadVolatile();
    }

    bool Shader::loadVolatile()
    {
        return true;
    }

    void Shader::unloadVolatile()
    {
        WHBGfxFreeShaderGroup(&this->program);
    }

    uint32_t Shader::getPixelSamplerLocation(int index)
    {
        if (this->shaderType != Shader::STANDARD_TEXTURE)
            throw love::Exception("Invalid shader set!");

        size_t count = this->program.pixelShader->samplerVarCount;

        if (index > count)
            throw love::Exception("Invalid sampler index");

        return this->program.pixelShader->samplerVars[index].location;
    }

    const Shader::UniformInfo Shader::getUniform(const std::string& name) const
    {
        const auto count = this->program.vertexShader->uniformBlockCount;

        for (size_t index = 0; index < count; index++)
        {
            const auto uniform = this->program.vertexShader->uniformBlocks[index];

            if (uniform.name == name)
                return UniformInfo { uniform.offset, name };
        }

        return {};
    }

    bool Shader::hasUniform(const std::string& name) const
    {
        return this->uniform.name == name;
    }

    void Shader::updateBuiltinUniforms(GraphicsBase* graphics, Uniform* uniform)
    {
        if (current != this)
            return;

        auto& transform = graphics->getTransform();
        // uniform->update(transform);

        GX2Invalidate(INVALIDATE_UNIFORM_BLOCK, uniform, UNIFORM_SIZE);
        GX2SetVertexUniformBlock(this->uniform.location, UNIFORM_SIZE, uniform);
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

    bool Shader::validate(const char* filepath, std::string& error)
    {
        FILE* file = std::fopen(filepath, "rb");

        if (!file)
        {
            error = "File does not exist.";
            std::fclose(file);
            return false;
        }

        std::unique_ptr<uint8_t[]> data;

        std::fseek(file, 0, SEEK_END);
        long size = std::ftell(file);
        std::rewind(file);

        try
        {
            data = std::make_unique<uint8_t[]>(size);
        }
        catch (std::bad_alloc&)
        {
            error = "Not enough memory.";
            return false;
        }

        long readSize = (long)std::fread(data.get(), 1, size, file);

        if (readSize != size)
        {
            error = "Failed to read whole file.";
            std::fclose(file);
            return false;
        }

        std::fclose(file);

        if (!WHBGfxLoadGFDShaderGroup(&this->program, 0, data.get()))
        {
            error = "Failed to load Shader Group";
            return false;
        }

        return true;
    }
} // namespace love