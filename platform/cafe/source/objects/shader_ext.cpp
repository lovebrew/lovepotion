#include <objects/shader_ext.hpp>

#include <utilities/driver/renderer_ext.hpp>

#include <gfd.h>
#include <gx2/mem.h>
#include <whb/gfx.h>

#include <malloc.h>

using namespace love;

#define SHADERS_DIR "/vol/content/shaders/"

#define DEFAULT_PRIMITIVE_SHADER (SHADERS_DIR "primitive.gfd")
#define DEFAULT_TEXTURE_SHADER   (SHADERS_DIR "texture.gfd")
#define DEFAULT_VIDEO_SHADER     (SHADERS_DIR "video.gfd")

Shader<Console::CAFE>::Shader() : program(nullptr)
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

    WHBGfxFreeShaderGroup(this->program);
}

uint32_t Shader<Console::CAFE>::GetPixelSamplerLocation(int index)
{
    if (this->shaderType != Shader::STANDARD_TEXTURE)
        throw love::Exception("Cannot fetch location from non-Texture pixel shader");

    return this->program->pixelShader->samplerVars[index].location;
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

static bool loadShaderFile(const char* filepath, Shader<>::StandardShader type,
                           WHBGfxShaderGroup*& program)
{
    FILE* file = fopen(filepath, "r");
    std::unique_ptr<uint8_t[]> data;

    std::fseek(file, 0, SEEK_END);
    long size = std::ftell(file);
    std::rewind(file);

    data = std::make_unique<uint8_t[]>(size);

    std::fread(data.get(), 1, size, file);
    std::fclose(file);

    return WHBGfxLoadGFDShaderGroup(program, 0, data.get());
}

void Shader<Console::CAFE>::LoadDefaults(StandardShader type)
{
    std::string error;

    switch (type)
    {
        case STANDARD_DEFAULT:
        default:
        {
            loadShaderFile(DEFAULT_PRIMITIVE_SHADER, type, this->program);
            break;
        }
        case STANDARD_TEXTURE:
        {
            loadShaderFile(DEFAULT_TEXTURE_SHADER, type, this->program);
            break;
        }
        case STANDARD_VIDEO:
        {
            break;
        }
    }

    WHBGfxInitShaderAttribute(this->program, "inPos", 0, 0, Shader::GX2_FORMAT_VEC3);
    WHBGfxInitShaderAttribute(this->program, "inColor", 0, 12, Shader::GX2_FORMAT_VEC4);
    WHBGfxInitShaderAttribute(this->program, "inTexCoord", 0, 28, Shader::GX2_FORMAT_VEC2);
    WHBGfxInitFetchShader(this->program);

    this->shaderType = type;
}
