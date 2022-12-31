#include <objects/shader_ext.hpp>

#include <utilities/driver/renderer_ext.hpp>

#include <gfd.h>
#include <gx2/mem.h>
#include <whb/gfx.h>

#include <malloc.h>

using namespace love;

#define SHADERS_DIR "/vol/content/shaders/"

#define DEFAULT_PRIMITIVE_SHADER (SHADERS_DIR "primitive.gsh")
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

    return this->program.pixelShader->samplerVars[index].location;
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

void Shader<Console::CAFE>::Attach()
{
    if (Shader::current != this)
    {
        Renderer<Console::CAFE>::Instance().UseProgram(this->program);
        Renderer<>::shaderSwitches++;

        Shader::current = this;
    }
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

    size_t readSize = std::fread(data.get(), 1, size, file);

    if (readSize != size)
    {
        error = "Failed to read whole file.";
        std::fclose(file);
        return false;
    }

    std::fclose(file);

    return WHBGfxLoadGFDShaderGroup(&program, 0, data.get());
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

    size_t offset = 0;
    WHBGfxInitShaderAttribute(&this->program, "inPos", 0, offset, Shader::GX2_FORMAT_VEC3);
    offset += (4 * 3);
    WHBGfxInitShaderAttribute(&this->program, "inColor", 0, offset, Shader::GX2_FORMAT_VEC4);
    offset += (4 * 4);
    WHBGfxInitShaderAttribute(&this->program, "inTexCoord", 0, offset,
                              Shader::GX2_FORMAT_UINT16_VEC2);
    WHBGfxInitFetchShader(&this->program);

    this->shaderType = type;
}
