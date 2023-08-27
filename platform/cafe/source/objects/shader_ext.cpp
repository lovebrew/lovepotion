#include <objects/shader_ext.hpp>

#include <utilities/driver/renderer_ext.hpp>
#include <utilities/driver/vertex_ext.hpp>

#include <gfd.h>
#include <gx2/mem.h>
#include <whb/gfx.h>

#include <malloc.h>

using namespace love;
using namespace vertex;

#define SHADERS_DIR "/vol/content/shaders/"

#define DEFAULT_PRIMITIVE_SHADER (SHADERS_DIR "color.gsh")
#define DEFAULT_TEXTURE_SHADER   (SHADERS_DIR "texture.gsh")
#define DEFAULT_VIDEO_SHADER     (SHADERS_DIR "video.gsh")

Shader<Console::CAFE>::Shader() : program {}
{}

Shader<Console::CAFE>::Shader(Data* group) : Shader()
{}

Shader<Console::CAFE>::~Shader()
{
    for (int i = 0; i < STANDARD_MAX_ENUM; i++)
    {
        if (this == Shader::defaults[i])
            Shader::defaults[i] = nullptr;
    }

    if (current == this)
        Shader::AttachDefault(STANDARD_DEFAULT);

    WHBGfxFreeShaderGroup(&this->program);
}

uint32_t Shader<Console::CAFE>::GetPixelSamplerLocation(int index)
{
    if (this->shaderType != Shader::STANDARD_TEXTURE)
        throw love::Exception("Cannot fetch location from non-Texture pixel shader");

    size_t count = this->program.pixelShader->samplerVarCount;

    if (index > count)
        throw love::Exception("Sampler variable #%d is out of bounds (max %d)", index, count);

    return this->program.pixelShader->samplerVars[index].location;
}

GX2UniformBlock* Shader<Console::CAFE>::GetUniformBlock(const char* name)
{
    return GX2GetVertexUniformBlock(this->program.vertexShader, name);
}

void Shader<Console::CAFE>::AttachDefault(StandardShader type)
{
    Shader* defaultshader = Shader::defaults[type];

    if (defaultshader == nullptr)
    {
        current = nullptr;
        return;
    }

    if (current != defaultshader)
        defaultshader->Attach();
}

void Shader<Console::CAFE>::Attach(bool forced)
{
    if (Shader::current != this || forced)
    {
        Renderer<Console::CAFE>::Instance().UseProgram(this->program);
        Renderer<>::shaderSwitches++;

        Shader::current = this;
    }
}

void Shader<Console::CAFE>::BindTexture(int index, GX2Texture* texture, GX2Sampler* sampler)
{
    const auto location = this->GetPixelSamplerLocation(index);

    GX2SetPixelTexture(texture, location);
    GX2SetPixelSampler(sampler, location);
}

static bool loadShaderFile(const char* filepath, WHBGfxShaderGroup& program, std::string& error)
{
    FILE* file = std::fopen(filepath, "r");

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

    if (!WHBGfxLoadGFDShaderGroup(&program, 0, data.get()))
    {
        error = "Failed to load Shader Group";
        return false;
    }

    return true;
}

void Shader<Console::CAFE>::LoadDefaults(StandardShader type)
{
    std::string error;
    bool success = false;

    switch (type)
    {
        case STANDARD_DEFAULT:
        default:
        {
            success = loadShaderFile(DEFAULT_PRIMITIVE_SHADER, this->program, error);
            break;
        }
        case STANDARD_TEXTURE:
        {
            success = loadShaderFile(DEFAULT_TEXTURE_SHADER, this->program, error);
            break;
        }
        case STANDARD_VIDEO:
        {
            /* todo */
            return;
        }
    }

    std::optional<const char*> shaderName;
    shaderName = Shader::standardShaders.ReverseFind(type);

    if (!success)
    {
        throw love::Exception("Failed to initialize the '%s' shader: %s", *shaderName,
                              error.c_str());
    }

    // clang-format off
    WHBGfxInitShaderAttribute(&this->program, "inPos",      0, POSITION_OFFSET, Shader::GX2_FORMAT_VEC3);
    WHBGfxInitShaderAttribute(&this->program, "inColor",    0, COLOR_OFFSET,    Shader::GX2_FORMAT_VEC4);
    WHBGfxInitShaderAttribute(&this->program, "inTexCoord", 0, TEXCOORD_OFFSET, Shader::GX2_FORMAT_VEC2);
    // clang-format on

    WHBGfxInitFetchShader(&this->program);

    this->shaderType = type;
}
