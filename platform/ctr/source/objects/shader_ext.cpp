#include <objects/shader_ext.hpp>

#include <common/exception.hpp>

#include <citro3d.h>

using namespace love;

#define SHADERS_DIR "romfs:/shaders/"

#define DEFAULT_SHADER (SHADERS_DIR "main_v_pica.shbin")

Shader<Console::CTR>::Shader() : uniforms {}
{}

Shader<Console::CTR>::Shader(Data* data) : uniforms {}
{
    std::string error;

    if (!this->Validate(data, error))
        throw love::Exception("Invalid shader: %s", error.c_str());
}

Shader<Console::CTR>::~Shader()
{
    for (int i = 0; i < STANDARD_MAX_ENUM; i++)
    {
        if (this == Shader::defaults[i])
            Shader::defaults[i] = nullptr;
    }

    if (Shader::current == this)
        Shader::AttachDefault(STANDARD_DEFAULT);

    shaderProgramFree(&this->program);
    DVLB_Free(this->binary);
}

void Shader<Console::CTR>::LoadDefaults(StandardShader)
{
    std::string error {};

    if (!this->Validate(DEFAULT_SHADER, error))
        throw love::Exception("Failed to load shader: %s.", error.c_str());

    shaderProgramInit(&this->program);
    shaderProgramSetVsh(&this->program, &this->binary->DVLE[0]);

    this->uniforms.uLocMdlView =
        shaderInstanceGetUniformLocation(this->program.vertexShader, "mdlvMtx");
    this->uniforms.uLocProjMtx =
        shaderInstanceGetUniformLocation(this->program.vertexShader, "projMtx");
}

void Shader<Console::CTR>::AttachDefault(StandardShader type)
{
    Shader* defaultShader = Shader::defaults[type];

    if (defaultShader == nullptr)
    {
        current = nullptr;
        return;
    }

    if (current != defaultShader)
        defaultShader->Attach();
}

void Shader<Console::CTR>::Attach()
{
    if (Shader::current != this)
    {
        C3D_BindProgram(&this->program);
        Shader::current = this;
    }
}

bool Shader<Console::CTR>::Validate(const char* filepath, std::string& error)
{
    FILE* file = std::fopen(filepath, "r");

    if (!file)
    {
        error = "File does not exist.";
        std::fclose(file);
        return false;
    }

    std::fseek(file, 0, SEEK_END);
    long size = std::ftell(file);
    std::rewind(file);

    try
    {
        this->data = std::make_unique<uint32_t[]>(size / sizeof(uint32_t));
    }
    catch (std::bad_alloc&)
    {
        error = "Not enough memory.";
        return false;
    }

    long readSize = (long)std::fread(this->data.get(), 1, size, file);

    if (readSize != size)
    {
        error = "Failed to read whole file.";
        std::fclose(file);
        return false;
    }

    std::fclose(file);
    this->binary = DVLB_ParseFile(this->data.get(), size);

    return true;
}

bool Shader<Console::CTR>::Validate(Data* data, std::string& error)
{
    this->data = std::make_unique<uint32_t[]>(data->GetSize() / 4);
    std::memcpy(this->data.get(), data->GetData(), data->GetSize());

    return true;
}