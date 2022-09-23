#include <utilities/driver/dsp_mem.hpp>

/* Audio Pool */
void* AudioMemory::POOL_BASE = nullptr;
AudioMemory::MemoryPool AudioMemory::audioPool;

bool AudioMemory::InitMemPool()
{
    if (!AudioMemory::POOL_BASE)
        AudioMemory::POOL_BASE = memalign(AUDREN_MEMPOOL_ALIGNMENT, AudioMemory::POOL_SIZE);

    if (!AudioMemory::POOL_BASE)
        return false;

    return true;
}

bool AudioMemory::Initialize()
{
    auto block = MemoryBlock::Create((uint8_t*)AudioMemory::POOL_BASE, AudioMemory::POOL_SIZE);

    if (!block)
        return false;

    audioPool.AddBlock(block);
    return true;
}

void* AudioMemory::Align(size_t size, size_t& aligned)
{
    if (!audioPool.Ready() && !Initialize())
        return nullptr;

    MemoryChunk chunk;
    if (!audioPool.Allocate(chunk, size))
        return nullptr;

    aligned = chunk.size;
    return chunk.address;
}

void AudioMemory::Free(const void* chunk, const size_t size)
{
    audioPool.DeAllocate((uint8_t*)chunk, size);
}

/* Audio Pool's Memory Pool */

void AudioMemory::MemoryPool::CoalesceRight(MemoryBlock* block)
{
    auto current = block->base + block->size;
    auto next    = block->next;

    for (auto n = next; n; n = next)
    {
        next = n->next;

        if (n->base != current)
            break;

        block->size += n->size;
        current += n->size;

        AudioMemory::MemoryPool::DeleteBlock(n);
    }
}

bool AudioMemory::MemoryPool::Allocate(MemoryChunk& chunk, size_t size)
{
    size_t alignMask = (AUDREN_BUFFER_ALIGNMENT - 1);

    if (size && alignMask)
    {
        if (size > UINTPTR_MAX - alignMask)
            return false;

        size = (size + alignMask) & ~alignMask;
    }

    for (auto block = first; block; block = block->next)
    {
        auto address = block->base;

        size_t waste = (size_t)address & alignMask;

        if (waste > 0)
            waste = alignMask + 1 - waste;

        if (waste > block->size)
            continue;

        address += waste;

        size_t blockSize = block->size - waste;

        if (blockSize < size)
            continue;

        // found space
        chunk.address = address;
        chunk.size    = size;

        if (!waste)
        {
            block->base += size;
            block->size -= size;

            if (!block->size)
                AudioMemory::MemoryPool::DeleteBlock(block);
        }
        else
        {
            auto nextAddress = address + size;
            auto nextSize    = blockSize - size;

            block->size = waste;

            if (nextSize)
            {
                auto next = MemoryBlock::Create(nextAddress, nextSize);

                if (next)
                    AudioMemory::MemoryPool::InsertAfter(block, next);
                else
                    chunk.size += nextSize;
            }
        }

        return true;
    }

    return false;
}

void AudioMemory::MemoryPool::DeAllocate(uint8_t* chunkAddress, size_t chunkSize)
{
    bool done = false;

    for (auto block = first; !done && block; block = block->next)
    {
        auto address = block->base;
        if (address > chunkAddress)
        {
            if ((chunkAddress + chunkSize) == address)
            {
                block->base = chunkAddress;
                block->size += chunkSize;
            }
            else
            {
                auto chunk = MemoryBlock::Create(chunkAddress, chunkSize);

                if (chunk)
                    AudioMemory::MemoryPool::InsertBefore(block, chunk);
            }

            done = true;
        }
        else if ((block->base + block->size) == chunkAddress)
        {
            block->size += chunkSize;
            AudioMemory::MemoryPool::CoalesceRight(block);

            done = true;
        }
    }

    if (!done)
    {
        auto block = MemoryBlock::Create(chunkAddress, chunkSize);

        if (block)
            AudioMemory::MemoryPool::AddBlock(block);
    }
}
