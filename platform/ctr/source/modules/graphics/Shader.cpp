#include "modules/graphics/Shader.hpp"
#include "driver/display/Renderer.hpp"

#define SHADERS_DIR "romfs:/shaders/"

#define DEFAULT_SHADER (SHADERS_DIR "main_v_pica.shbin")

namespace love
{
    Shader::Shader() : locations {}
    {
        this->loadVolatile();
    }

    Shader::~Shader()
    {
        this->unloadVolatile();
    }

    bool Shader::loadVolatile()
    {
        if (this->dvlb != nullptr)
            return true;

        std::string error;

        if (!this->validate(DEFAULT_SHADER, error))
            throw love::Exception("Failed to load default shader: {:s}", error);

        shaderProgramInit(&this->program);
        shaderProgramSetVsh(&this->program, &this->dvlb->DVLE[0]);

        this->locations.mdlvMtx = shaderInstanceGetUniformLocation(this->program.vertexShader, "mdlvMtx");
        this->locations.projMtx = shaderInstanceGetUniformLocation(this->program.vertexShader, "projMtx");

        return true;
    }

    void Shader::unloadVolatile()
    {
        shaderProgramFree(&this->program);
        DVLB_Free(this->dvlb);
    }

    void Shader::attach()
    {
        if (current == this)
            return;

        C3D_BindProgram(&this->program);
        current = this;
    }

    ptrdiff_t Shader::getHandle() const
    {
        return 0;
    }

    bool Shader::validate(const char* filepath, std::string& error)
    {
        std::FILE* file = std::fopen(filepath, "rb");

        if (file == nullptr)
        {
            error = "Failed to open file.";
            std::fclose(file);
            return false;
        }

        std::fseek(file, 0, SEEK_END);
        long size = std::ftell(file);
        std::rewind(file);

        try
        {
            this->data.resize(size / sizeof(uint32_t));
        }
        catch (std::bad_alloc&)
        {
            error = E_OUT_OF_MEMORY;
            std::fclose(file);
            return false;
        }

        long read = std::fread(this->data.data(), 1, size, file);

        if (read != size)
        {
            error = "Failed to read file.";
            std::fclose(file);
            return false;
        }

        std::fclose(file);
        this->dvlb = DVLB_ParseFile(this->data.data(), size);

        if (this->dvlb == nullptr)
        {
            error = "Failed to parse DVLB.";
            std::fclose(file);
            return false;
        }

        return true;
    }
} // namespace love
