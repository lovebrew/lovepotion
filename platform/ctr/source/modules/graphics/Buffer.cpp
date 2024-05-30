#include "modules/graphics/Buffer.hpp"

namespace love
{
    Buffer::Buffer(GraphicsBase* graphics, const Settings& settings,
                   const std::vector<DataDeclaration>& format, const void* data, size_t size,
                   size_t arraylength) :
        BufferBase(graphics, settings, format, size, arraylength)
    {
        this->size  = this->getSize();
        arraylength = this->getArrayLength();

        if (usageFlags & BUFFERUSAGE_VERTEX)
            mapUsage = BUFFERUSAGE_VERTEX;
        else if (usageFlags & BUFFERUSAGE_INDEX)
            mapUsage = BUFFERUSAGE_INDEX;

        if (dataUsage == BUFFERDATAUSAGE_STREAM)
            this->ownsMemoryMap = true;

        std::vector<uint8_t> empty {};

        try
        {
            empty.resize(this->getSize());
            data = empty.data();
        }
        catch (std::exception&)
        {
            data = nullptr;
        }

        if (!this->load(data))
        {
            this->unloadVolatile();
            throw love::Exception("Could not create buffer with %d bytes (out of VRAM?)", size);
        }
    }
} // namespace love
