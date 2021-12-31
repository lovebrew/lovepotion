#include "objects/compressedimagedata/types/compressedmemory.h"
#include "common/exception.h"

using namespace love;

CompressedMemory::CompressedMemory(size_t size) : data(nullptr), size(size)
{
    try
    {
        data = new uint8_t[size];
    }
    catch (const std::exception&)
    {
        throw love::Exception("Out of memory.");
    }
}

CompressedMemory::~CompressedMemory()
{
    if (this->data != nullptr)
        delete[] data;
}
