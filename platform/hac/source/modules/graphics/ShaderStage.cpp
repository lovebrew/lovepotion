#include "common/Exception.hpp"

#include "driver/display/deko.hpp"
#include "modules/graphics/ShaderStage.hpp"

struct DkshHeader
{
    uint32_t magic;
    uint32_t header_sz;
    uint32_t control_sz;
    uint32_t code_sz;
    uint32_t programs_off;
    uint32_t num_programs;
};

namespace love
{
    ShaderStage::ShaderStage(ShaderStageType stage, const std::string& filepath) :
        ShaderStageBase(stage, filepath)
    {
        this->loadVolatile();
    }

    ShaderStage::~ShaderStage()
    {
        this->unloadVolatile();
    }

    bool ShaderStage::loadVolatile()
    {
        if (this->memory)
            return true;

        std::FILE* file = std::fopen(this->filepath.c_str(), "rb");

        if (!file)
        {
            this->warnings.append("Failed to open file {:s}", this->filepath.c_str());
            return false;
        }

        DkshHeader header {};
        void* controlMemory = nullptr;

        this->memory.destroy();

        if (!std::fread(&header, sizeof(header), 1, file))
        {
            std::fclose(file);
            this->warnings.append(std::format("Failed to read file {:s}", this->filepath));
            return false;
        }

        controlMemory = std::malloc(header.control_sz);

        if (!controlMemory)
        {
            std::fclose(file);
            this->warnings.append(E_OUT_OF_MEMORY);
            return false;
        }

        std::rewind(file);

        if (!std::fread(controlMemory, header.control_sz, 1, file))
        {
            std::free(controlMemory);
            std::fclose(file);
            this->warnings.append("Failed to read shader control code.");
            return false;
        }

        auto& pool   = d3d.getMemoryPool(deko3d::MEMORYPOOL_CODE);
        this->memory = pool.allocate(header.code_sz, DK_SHADER_CODE_ALIGNMENT);

        if (!this->memory)
        {
            std::free(controlMemory);
            std::fclose(file);
            this->warnings.append(E_OUT_OF_MEMORY);
            return false;
        }

        if (!std::fread(this->memory.getCpuAddr(), header.code_sz, 1, file))
        {
            this->memory.destroy();
            std::free(controlMemory);
            std::fclose(file);
            this->warnings.append("Failed to read shader code into CPU memory.");
            return false;
        }

        dk::ShaderMaker { this->memory.getMemBlock(), this->memory.getOffset() }
            .setProgramId(0)
            .setControl(controlMemory)
            .initialize(this->shader);

        std::free(controlMemory);
        std::fclose(file);

        return true;
    }

    void ShaderStage::unloadVolatile()
    {
        this->memory.destroy();
    }

    ptrdiff_t ShaderStage::getHandle() const
    {
        return (ptrdiff_t)std::addressof(this->shader);
    }
} // namespace love
