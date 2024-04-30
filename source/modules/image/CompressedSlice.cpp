#include "common/Exception.hpp"

#include "modules/image/CompressedSlice.hpp"

namespace love
{
    CompressedSlice::CompressedSlice(PixelFormat format, int width, int height, ByteData* memory,
                                     size_t offset, size_t size) :
        ImageDataBase(format, width, height),
        memory(memory),
        offset(offset),
        size(size)
    {}

    CompressedSlice::CompressedSlice(const CompressedSlice& other) :
        ImageDataBase(other.getFormat(), other.getWidth(), other.getHeight()),
        memory(other.memory),
        offset(other.offset),
        size(other.size)
    {}

    CompressedSlice::~CompressedSlice()
    {}

    CompressedSlice* CompressedSlice::clone() const
    {
        return new CompressedSlice(*this);
    }
} // namespace love
