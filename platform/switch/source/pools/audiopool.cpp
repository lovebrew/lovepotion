#include "pools/audiopool.h"

/* Audio Pool */
void* AudioPool::AUDIO_POOL_BASE;
AudioPool::MemoryPool AudioPool::audioPool;

bool AudioPool::Initialize()
{
    auto block = MemoryBlock::Create((u8*)AUDIO_POOL_BASE, AUDIO_POOL_SIZE);

    if (block)
    {
        audioPool.AddBlock(block);

        return true;
    }

    return false;
}

std::pair<void*, size_t> AudioPool::MemoryAlign(size_t size)
{
    if (!audioPool.Ready() && !Initialize())
        return std::pair(nullptr, -1);

    MemoryChunk chunk;
    if (!audioPool.Allocate(chunk, size))
        return std::pair(nullptr, -2);

    return std::pair(chunk.address, chunk.size);
}

void AudioPool::MemoryFree(const std::pair<void*, size_t>& chunk)
{
    audioPool.DeAllocate((u8*)chunk.first, chunk.second);
}

/* Audio Pool's Memory Pool */

void AudioPool::MemoryPool::CoalesceRight(MemoryBlock* block)
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

        AudioPool::MemoryPool::DeleteBlock(n);
    }
}

bool AudioPool::MemoryPool::Allocate(MemoryChunk& chunk, size_t size)
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
                AudioPool::MemoryPool::DeleteBlock(block);
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
                    AudioPool::MemoryPool::InsertAfter(block, next);
                else
                    chunk.size += nextSize;
            }
        }

        return true;
    }

    return false;
}

void AudioPool::MemoryPool::DeAllocate(u8* chunkAddress, size_t chunkSize)
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
                    AudioPool::MemoryPool::InsertBefore(block, chunk);
            }

            done = true;
        }
        else if ((block->base + block->size) == chunkAddress)
        {
            block->size += chunkSize;
            AudioPool::MemoryPool::CoalesceRight(block);

            done = true;
        }
    }

    if (!done)
    {
        auto block = MemoryBlock::Create(chunkAddress, chunkSize);

        if (block)
            AudioPool::MemoryPool::AddBlock(block);
    }
}
