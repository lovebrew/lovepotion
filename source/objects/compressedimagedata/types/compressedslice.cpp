#include "objects/compressedimagedata/types/compressedslice.h"
#include "common/exception.h"

using namespace love;

CompressedSlice::CompressedSlice(PixelFormat format, int width, int height,
                                 CompressedMemory* memory, size_t offset, size_t size) :
    ImageDataBase(format, width, height),
    memory(memory),
    offset(offset),
    dataSize(size)
{}

CompressedSlice::CompressedSlice(const CompressedSlice& s) :
    ImageDataBase(s.GetFormat(), s.GetWidth(), s.GetHeight()),
    memory(s.memory),
    offset(s.offset),
    dataSize(s.dataSize)
{}

CompressedSlice::~CompressedSlice()
{}

CompressedSlice* CompressedSlice::Clone() const
{
    return new CompressedSlice(*this);
}
