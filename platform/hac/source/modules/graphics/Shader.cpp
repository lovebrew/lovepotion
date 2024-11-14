#include "modules/graphics/Shader.hpp"
#include "driver/display/deko.hpp"

#include <cstring>

#define SHADERS_DIR "romfs:/shaders/"

#define DEFAULT_VERTEX_SHADER   (SHADERS_DIR "transform_vsh.dksh")
#define DEFAULT_FRAGMENT_SHADER (SHADERS_DIR "color_fsh.dksh")
#define DEFAULT_TEXTURE_SHADER  (SHADERS_DIR "texture_fsh.dksh")
#define DEFAULT_VIDEO_SHADER    (SHADERS_DIR "video_fsh.dksh")

namespace love
{
    struct DkshHeader
    {
        uint32_t magic;
        uint32_t header_sz;
        uint32_t control_sz;
        uint32_t code_sz;
        uint32_t programs_off;
        uint32_t num_programs;
    };

    Shader::Shader()
    {}

    Shader::Shader(StandardShader shader) : ShaderBase(shader)
    {
        std::string error;
        const char* fragmentStage = nullptr;

        switch (shader)
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

        if (!this->validate(this->program.vertex, DEFAULT_VERTEX_SHADER, error))
            throw love::Exception("Invalid vertex shader: {:s}", error);

        error.clear();

        if (!this->validate(this->program.fragment, fragmentStage, error))
            throw love::Exception("Invalid fragment shader: {:s}", error);
    }

    Shader::~Shader()
    {
        this->unloadVolatile();
    }

    void Shader::attach()
    {
        if (Shader::current != this)
        {
            Graphics::flushBatchedDrawsGlobal();

            d3d.useProgram(this->program.vertex.shader, this->program.fragment.shader);
            ++shaderSwitches;

            Shader::current = this;
        }
    }

    bool Shader::loadVolatile()
    {
        return true;
    }

    void Shader::unloadVolatile()
    {
        this->program.vertex.memory.destroy();
        this->program.fragment.memory.destroy();
    }

    void Shader::updateBuiltinUniforms(GraphicsBase* graphics)
    {}

    ptrdiff_t Shader::getHandle() const
    {
        return 0;
    }

    static bool loadFile(Shader::Stage& stage, const uint8_t* buffer, size_t size, std::string& error)
    {
        DkshHeader header {};
        std::unique_ptr<uint8_t[]> control = nullptr;

        stage.memory.destroy();

        if (!buffer || size == 0)
        {
            error = "Buffer size is zero.";
            return false;
        }

        const auto headerSize = sizeof(DkshHeader);
        std::memcpy(&header, buffer, headerSize);

        if (header.header_sz != headerSize)
        {
            error = "Invalid header size.";
            return false;
        }

        try
        {
            control = std::make_unique<uint8_t[]>(header.control_sz);
        }
        catch (std::bad_alloc&)
        {
            error = E_OUT_OF_MEMORY;
            return false;
        }

        std::memcpy(control.get(), buffer, header.control_sz);

        auto& pool   = d3d.getMemoryPool(deko3d::MEMORYPOOL_CODE);
        stage.memory = pool.allocate(header.code_sz, DK_SHADER_CODE_ALIGNMENT);

        if (!stage.memory)
        {
            error = "Failed to allocate memory.";
            return false;
        }

        std::memcpy(stage.memory.getCpuAddr(), buffer + header.control_sz, header.code_sz);

        dk::ShaderMaker { stage.memory.getMemBlock(), stage.memory.getOffset() }
            .setControl(control.get())
            .setProgramId(0)
            .initialize(stage.shader);

        if (!stage.shader.isValid())
        {
            error = "Failed to initialize shader.";
            stage.memory.destroy();
            return false;
        }

        return true;
    }

    bool Shader::validate(Stage& stage, const char* filepath, std::string& error)
    {
        if (!filepath)
        {
            error = "No filepath provided.";
            return false;
        }

        FILE* file = fopen(filepath, "rb");

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
            error = "File '" + std::string(filepath) + "' is empty.";
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
            error = E_OUT_OF_MEMORY;
            std::fclose(file);
            return false;
        }

        std::fread(buffer.get(), size, 1, file);
        std::fclose(file);

        return loadFile(stage, buffer.get(), size, error);
    }
} // namespace love
