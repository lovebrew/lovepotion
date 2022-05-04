#include "objects/compressedimagedata/types/compressedslice.h"
#include "common/exception.h"

using namespace love;

CompressedSlice::CompressedSlice(PixelFormat format, int width, int height, ByteData* memory,
                                 size_t offset, size_t size) :
    ImageDataBase(format, width, height),
    memory(memory),
    offset(offset),
    dataSize(size),
    sRGB(false)
{}

CompressedSlice::CompressedSlice(const CompressedSlice& s) :
    ImageDataBase(s.GetFormat(), s.GetWidth(), s.GetHeight()),
    memory(s.memory),
    offset(s.offset),
    dataSize(s.dataSize),
    sRGB(s.sRGB)
{}

CompressedSlice::~CompressedSlice()
{}

CompressedSlice* CompressedSlice::Clone() const
{
    return new CompressedSlice(*this);
}
