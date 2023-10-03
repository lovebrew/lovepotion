#include <objects/compressedimagedata/compressedslice.hpp>

#include <common/exception.hpp>

using namespace love;

CompressedSlice::CompressedSlice(PixelFormat format, int width, int height, ByteData* memory,
                                 size_t offset, size_t size) :
    ImageDataBase(format, width, height),
    memory(memory),
    offset(offset),
    dataSize(size),
    sRGB(false)
{}

CompressedSlice::CompressedSlice(const CompressedSlice& slice) :
    ImageDataBase(slice.format, slice.width, slice.height),
    memory(slice.memory),
    offset(slice.offset),
    dataSize(slice.dataSize),
    sRGB(slice.sRGB)
{}

CompressedSlice* CompressedSlice::Clone() const
{
    return new CompressedSlice(*this);
}
