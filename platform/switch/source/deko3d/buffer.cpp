#include "deko3d/buffer.h"
#include "deko3d/deko.h"

#include <limits>

#include "common/exception.h"

using namespace love;

Buffer::Buffer(size_t size, const void* data, vertex::BufferType type, vertex::Usage usage,
               uint32_t mapFlags) :
    size(size),
    type(type),
    usage(usage),
    mapFlags(mapFlags),
    isMapped(false),
    modifiedStart(std::numeric_limits<size_t>::max()),
    modifiedEnd(0)
{
    this->memblock = dk::MemBlockMaker(::deko3d::Instance().GetDevice(), size)
                         .setFlags(DkMemBlockFlags_CpuUncached)
                         .create();

    if (data != nullptr)
        memcpy(this->memblock.getCpuAddr(), data, size);

    if (!this->Load(data != nullptr))
    {
        this->memblock.destroy();
        throw love::Exception("Could not load vertex buffer (out of VRAM?)");
    }
}

Buffer::~Buffer()
{
    this->memblock.destroy();
}

void* Buffer::Map()
{
    if (this->isMapped)
        return this->memblock.getCpuAddr();

    this->isMapped = true;

    this->modifiedStart = std::numeric_limits<size_t>::max();
    this->modifiedEnd   = 0;

    return this->memblock.getCpuAddr();
}

void Buffer::UnmapStatic(size_t offset, size_t size)
{
    if (size == 0)
        return;

    this->memblock.flushCpuCache(offset, size);
}

void Buffer::UnmapStream()
{
    memset(this->memblock.getCpuAddr(), 0, this->GetSize());
    this->memblock.flushCpuCache(0, this->GetSize());
}

void Buffer::Unmap()
{
    if (!this->isMapped)
        return;
}

void Buffer::SetMappedRangeModified(size_t offset, size_t modifiedSize)
{}

void Buffer::Fill(size_t offset, size_t size, const void* data)
{}

void Buffer::CopyTo(size_t offset, size_t size, Buffer* other, size_t otherOffset)
{
    other->Fill(otherOffset, size, this->memblock.getCpuAddr());
}

bool Buffer::LoadVolatile()
{
    return this->Load(true);
}

void Buffer::UnloadVolatile()
{
    this->Unload();
}

bool Buffer::Load(bool restore)
{}
