#include "common/Exception.hpp"

#include "modules/graphics/ShaderStage.hpp"

#include <memory>

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

        if (!file)
        {
            this->warnings.append(std::format("Failed to open file {:s}", this->filepath.c_str()));
            return false;
        }

        std::fseek(file, 0, SEEK_END);
        long size = std::ftell(file);
        std::rewind(file);

        if (size <= 0)
        {
            this->warnings.append("Invalid file size.");
            std::fclose(file);
            return false;
        }

        try
        {
            this->code.resize(size);
        }
        catch (std::bad_alloc&)
        {
            std::fclose(file);
            this->warnings.append(E_OUT_OF_MEMORY);
            return false;
        }

        size_t read = std::fread(this->code.data(), size, 1, file);

        if (read < 1)
        {
            this->warnings.append(std::format("Failed to read file '{}'.", this->filepath.c_str()));
            std::fclose(file);
            return false;
        }

        std::fclose(file);

        if (this->getStageType() == SHADERSTAGE_VERTEX)
        {
            this->vertex = WHBGfxLoadGFDVertexShader(0, this->code.data());

            if (!this->vertex)
            {
                this->warnings.append("Failed to load Vertex Shader.");
                return false;
            }
        }

        if (this->getStageType() == SHADERSTAGE_PIXEL)
        {
            this->pixel = WHBGfxLoadGFDPixelShader(0, this->code.data());

            if (!this->pixel)
            {
                this->warnings.append("Failed to load Pixel Shader.");
                return false;
            }
        }

        return true;
    }

    void ShaderStage::unloadVolatile()
    {}

    ptrdiff_t ShaderStage::getHandle() const
    {
        if (this->getStageType() == SHADERSTAGE_VERTEX)
            return (ptrdiff_t)this->vertex;

        return (ptrdiff_t)this->pixel;
    }
} // namespace love
