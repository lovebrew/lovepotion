#include <objects/shader_ext.hpp>

#include <utilities/driver/renderer_ext.hpp>

using namespace love;

#define SHADERS_DIR "romfs:/shaders/"

#define DEFAULT_VERTEX_SHADER   (SHADERS_DIR "transform_vsh.dksh")
#define DEFAULT_FRAGMENT_SHADER (SHADERS_DIR "color_fsh.dksh")
#define DEFAULT_TEXTURE_SHADER  (SHADERS_DIR "texture_fsh.dksh")
#define DEFAULT_VIDEO_SHADER    (SHADERS_DIR "video_fsh.dksh")

Shader<Console::HAC>::Shader() : program {}
{}

Shader<Console::HAC>::Shader(Data* vertex, Data* fragment) : program {}
{
    if (!this->Validate(vertex, *this->program.vertex))
        throw love::Exception("Invalid vertex shader.");

    if (!this->Validate(fragment, *this->program.fragment))
        throw love::Exception("Invalid fragment shader");
}

Shader<Console::HAC>::~Shader()
{
    for (int i = 0; i < STANDARD_MAX_ENUM; i++)
    {
        if (this == Shader::defaults[i])
            Shader::defaults[i] = nullptr;
    }

    if (current == this)
        Shader::AttachDefault(STANDARD_DEFAULT);
}

void Shader<Console::HAC>::LoadDefaults(StandardShader type)
{
    const char* fragmentStage = nullptr;

    switch (type)
    {
        case STANDARD_DEFAULT:
        {
            fragmentStage = DEFAULT_FRAGMENT_SHADER;
            break;
        }
        case STANDARD_TEXTURE:
        {
            fragmentStage = DEFAULT_TEXTURE_SHADER;
            break;
        }
        case STANDARD_VIDEO:
        {
            fragmentStage = DEFAULT_VIDEO_SHADER;
            break;
        }
        default:
            break;
    }

    if (!this->Validate(DEFAULT_VERTEX_SHADER, *this->program.vertex))
        throw love::Exception("Invalid vertex shader.");

    if (!this->Validate(fragmentStage, *this->program.fragment))
        throw love::Exception("Invalid fragment shader.");
}

void Shader<Console::HAC>::AttachDefault(StandardShader type)
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

void Shader<Console::HAC>::Attach()
{
    if (Shader::current != this)
    {
        Renderer<Console::HAC>::Instance().UseProgram(this);
        Renderer<>::shaderSwitches++;

        Shader::current = this;
    }
}

#include <utilities/log/logfile.h>

static bool loadFile(Shader<Console::HAC>::DekoStage& stage, const uint8_t* buffer,
                     const size_t size)
{
    Shader<Console::HAC>::DkshHeader header {};
    std::unique_ptr<uint8_t[]> controlMemory;

    stage.codeMemory.destroy();

    if (!buffer || size == 0)
        return false;

    if (!std::memcpy(&header, buffer, sizeof(header)))
        return false;

    try
    {
        controlMemory = std::make_unique<uint8_t[]>(header.control_sz);
    }
    catch (std::bad_alloc&)
    {
        return false;
    }

    if (!std::memcpy(controlMemory.get(), buffer, header.control_sz))
        return false;

    const auto poolId = Renderer<Console::HAC>::CODE;
    auto& pool        = Renderer<Console::HAC>::Instance().GetMemPool(poolId);

    stage.codeMemory = pool.allocate(header.code_sz, DK_SHADER_CODE_ALIGNMENT);

    if (!stage.codeMemory)
        return false;

    if (!std::memcpy(stage.codeMemory.getCpuAddr(), buffer + header.control_sz, header.code_sz))
    {
        stage.codeMemory.destroy();

        return false;
    }

    dk::ShaderMaker { stage.codeMemory.getMemBlock(), stage.codeMemory.getOffset() }
        .setControl(controlMemory.get())
        .setProgramId(0)
        .initialize(stage.shader);

    return true;
}

bool Shader<Console::HAC>::Validate(const char* filepath, DekoStage& stage) const
{
    if (!filepath)
        return false;

    FILE* file = std::fopen(filepath, "r");

    if (!file)
    {
        std::fclose(file);
        return false;
    }

    std::fseek(file, 0, SEEK_END);

    size_t size = std::ftell(file);

    if (size == 0)
    {
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
        std::fclose(file);
        return false;
    }

    std::fwrite(buffer.get(), size, 1, file);
    fclose(file);

    return loadFile(stage, buffer.get(), size);
}

bool Shader<Console::HAC>::Validate(Data* data, DekoStage& stage) const
{
    return loadFile(stage, (uint8_t*)data->GetData(), data->GetSize());
}
