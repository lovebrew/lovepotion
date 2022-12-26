#include <objects/shader_ext.hpp>

#include <utilities/driver/renderer_ext.hpp>

using namespace love;

#define SHADERS_DIR "/vol/content/shaders/"

#define DEFAULT_PRIMITIVE_SHADER (SHADERS_DIR "primitive.gsh")
#define DEFAULT_TEXTURE_SHADER   (SHADERS_DIR "texture.gsh")

Shader<Console::CAFE>::Shader() : group {}
{}

Shader<Console::CAFE>::Shader(Data* vertex, Data* fragment)
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
        Renderer<Console::CAFE>::Instance().UseProgram(this->group);
        Renderer<>::shaderSwitches++;

        Shader::current = this;
    }
}

void Shader<Console::CAFE>::LoadDefaults(StandardShader type)
{
    std::string error;

    switch (type)
    {
        case STANDARD_DEFAULT:
        default:
        {
            this->Validate(DEFAULT_PRIMITIVE_SHADER, this->group, error);

            // clang-format off
            WHBGfxInitShaderAttribute(&this->group, "position", 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32);
            WHBGfxInitShaderAttribute(&this->group, "color",    0, 16, GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32);
            // clang-format on

            break;
        }
        case STANDARD_TEXTURE:
        {
            this->Validate(DEFAULT_TEXTURE_SHADER, this->group, error);

            // clang-format off
            WHBGfxInitShaderAttribute(&this->group, "position", 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32);
            WHBGfxInitShaderAttribute(&this->group, "color",    0, 16, GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32);
            // clang-format on

            break;
        }
    }

    if (!error.empty())
        throw love::Exception("Invalid Shader group: %s", error.c_str());
}

bool Shader<Console::CAFE>::Validate(const char* filepath, WHBGfxShaderGroup& stage,
                                     std::string& error) const
{
    if (!filepath)
    {
        error = "No filepath provided.";
        return false;
    }

    FILE* file = std::fopen(filepath, "rb");

    if (!file)
    {
        error = "File '" + std::string(filepath) + "' does not exist.";
        std::fclose(file);
        return false;
    }

    std::fseek(file, 0, SEEK_END);

    size_t size = std::ftell(file);

    if (size == 0)
    {
        error = "File size is zero.";
        std::fclose(file);
        return false;
    }

    std::rewind(file);

    std::unique_ptr<uint8_t[]> buffer = nullptr;

    try
    {
        buffer = std::make_unique<uint8_t[]>(size);
    }
    catch (std::bad_alloc&)
    {
        error = "Failed to allocate buffer.";
        std::fclose(file);
        return false;
    }

    std::fread(buffer.get(), size, 1, file);
    fclose(file);

    if (!WHBGfxLoadGFDShaderGroup(&stage, 0, buffer.get()))
    {
        error = "Invalid shader group.";
        return false;
    }

    return true;
}
