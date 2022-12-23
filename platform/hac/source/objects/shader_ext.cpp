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
    std::string error;

    if (!this->Validate(vertex, *this->program.vertex, error))
        throw love::Exception("Invalid vertex shader: %s", error.c_str());

    error.clear();

    if (!this->Validate(fragment, *this->program.fragment, error))
        throw love::Exception("Invalid fragment shader: %s", error.c_str());
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

    this->program.vertex->codeMemory.destroy();
    this->program.fragment->codeMemory.destroy();
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

    std::string error;
    if (!this->Validate(DEFAULT_VERTEX_SHADER, *this->program.vertex, error))
        throw love::Exception("Invalid vertex shader: %s", error.c_str());

    error.clear();

    if (!this->Validate(fragmentStage, *this->program.fragment, error))
        throw love::Exception("Invalid fragment shader: %s", error.c_str());
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
        Renderer<Console::HAC>::Instance().UseProgram(this->program);
        Renderer<>::shaderSwitches++;

        Shader::current = this;
    }
}

static bool loadFile(Shader<Console::HAC>::DekoStage& stage, const uint8_t* buffer,
                     const size_t size, std::string& error)
{
    Shader<Console::HAC>::DkshHeader header {};
    std::unique_ptr<uint8_t[]> controlMemory;

    stage.codeMemory.destroy();

    if (!buffer || size == 0)
    {
        error = "Buffer size is zero.";
        return false;
    }

    const auto headerSize = Shader<Console::HAC>::HEADER_SIZE;
    std::memcpy(&header, buffer, headerSize);

    if (header.header_sz != headerSize)
    {
        error = "Invalid dksh header size: expected " + std::to_string(headerSize) + ", got " +
                std::to_string(header.header_sz);
        return false;
    }

    try
    {
        controlMemory = std::make_unique<uint8_t[]>(header.control_sz);
    }
    catch (std::bad_alloc&)
    {
        error = "Failed to allocate control memory.";
        return false;
    }

    std::memcpy(controlMemory.get(), buffer, header.control_sz);

    const auto poolId = Renderer<Console::HAC>::CODE;
    auto& pool        = Renderer<Console::HAC>::Instance().GetMemPool(poolId);

    stage.codeMemory = pool.allocate(header.code_sz, DK_SHADER_CODE_ALIGNMENT);

    if (!stage.codeMemory)
    {
        error = "Failed to allocate code memory.";
        return false;
    }

    std::memcpy(stage.codeMemory.getCpuAddr(), buffer + header.control_sz, header.code_sz);

    dk::ShaderMaker { stage.codeMemory.getMemBlock(), stage.codeMemory.getOffset() }
        .setControl(controlMemory.get())
        .setProgramId(0)
        .initialize(stage.shader);

    if (!stage.shader.isValid())
    {
        error = "Shader code is invalid.";
        stage.codeMemory.destroy();
        return false;
    }

    return true;
}

bool Shader<Console::HAC>::Validate(const char* filepath, DekoStage& stage,
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

    return loadFile(stage, buffer.get(), size, error);
}

bool Shader<Console::HAC>::Validate(Data* data, DekoStage& stage, std::string& error) const
{
    return loadFile(stage, (uint8_t*)data->GetData(), data->GetSize(), error);
}
