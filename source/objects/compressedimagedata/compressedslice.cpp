#include <objects/compressedimagedata/compressedslice.hpp>

#include <common/exception.hpp>

using namespace love;

CompressedSlice::CompressedSlice(PixelFormat format, int width, int height, ByteData* memory,
                                 size_t offset, size_t size) :
    ImageData<Console::Which>(format, width, height),
    memory(memory),
    offset(offset),
    dataSize(size)
{}

CompressedSlice::CompressedSlice(const CompressedSlice& slice) :
    ImageData<Console::Which>(slice.format, slice.width, slice.height),
    memory(slice.memory),
    offset(slice.offset),
    dataSize(slice.dataSize)
{}

CompressedSlice* CompressedSlice::Clone() const
{
    return new CompressedSlice(*this);
}
