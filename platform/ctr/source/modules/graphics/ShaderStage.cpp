#include "common/error.hpp"

#include "modules/graphics/ShaderStage.hpp"

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
        std::FILE* file = std::fopen(this->filepath.c_str(), "rb");

        if (file == nullptr)
        {
            this->warnings.append("Failed to open file.");
            std::fclose(file);
            return false;
        }

        std::fseek(file, 0, SEEK_END);
        long size = std::ftell(file);
        std::rewind(file);

        try
        {
            this->code.resize(size / sizeof(uint32_t));
        }
        catch (std::bad_alloc&)
        {
            this->warnings.append(E_OUT_OF_MEMORY);
            std::fclose(file);
            return false;
        }

        long read = std::fread(this->code.data(), 1, size, file);

        if (read != size)
        {
            this->warnings.append("Failed to read file.");
            std::fclose(file);
            return false;
        }

        std::fclose(file);

        this->dvlb = DVLB_ParseFile(this->code.data(), size);

        if (this->dvlb == nullptr)
        {
            this->warnings.append("Failed to parse DVLB.");
            std::fclose(file);
            return false;
        }

        return true;
    }

    void ShaderStage::unloadVolatile()
    {
        DVLB_Free(this->dvlb);
    }

    ptrdiff_t ShaderStage::getHandle() const
    {
        return (ptrdiff_t)this->dvlb;
    }
} // namespace love
