#include "modules/graphics/Shader.hpp"
#include "driver/display/citro3d.hpp"

#define SHADERS_DIR "romfs:/shaders/"

#define DEFAULT_SHADER (SHADERS_DIR "main_v_pica.shbin")

namespace love
{
    Shader::Shader() : uniforms {}
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

        this->uniforms[0] = this->getUniform("mdlvMtx");
        this->uniforms[1] = this->getUniform("projMtx");

        return true;
    }

    void Shader::unloadVolatile()
    {
        shaderProgramFree(&this->program);
        DVLB_Free(this->dvlb);
    }

    const Shader::UniformInfo Shader::getUniform(const std::string& name) const
    {
        int8_t location = 0;
        if ((location = shaderInstanceGetUniformLocation(this->program.vertexShader, name.c_str())) < 0)
            throw love::Exception("Failed to get uniform location: {:s}", name);

        return { location, name };
    }

    bool Shader::hasUniform(const std::string& name) const
    {
        for (int index = 0; index < 2; index++)
        {
            if (this->uniforms[index].name == name)
                return true;
        }

        return false;
    }

    static void updateTransform(C3D_Mtx& matrix, const Matrix4& transform)
    {
        for (int row = 0; row < 4; row++)
        {
            for (int column = 0; column < 4; column++)
                matrix.m[row * 4 + (3 - column)] = transform.get(row, column);
        }
    }

    void Shader::updateBuiltinUniforms(GraphicsBase* graphics, C3D_Mtx mdlvMtx, const C3D_Mtx& projMtx)
    {
        if (this->hasUniform("mdlvMtx"))
        {
            updateTransform(mdlvMtx, graphics->getTransform());
            C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, this->uniforms[0].location, &mdlvMtx);
        }

        if (this->hasUniform("projMtx"))
            C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, this->uniforms[1].location, &projMtx);
    }

    void Shader::attach()
    {
        if (current != this)
        {
            Graphics::flushBatchedDrawsGlobal();

            C3D_BindProgram(&this->program);
            current = this;
        }
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
