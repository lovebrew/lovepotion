#include "common/runtime.h"
#include "common/pool.h"

/* Audio Pool */
void * AudioPool::AUDIO_POOL_BASE;
AudioPool::MemoryPool AudioPool::audioPool;

bool AudioPool::Initialize()
{
    AUDIO_POOL_BASE = memalign(AUDREN_MEMPOOL_ALIGNMENT, AUDIO_POOL_SIZE);

    auto block = MemoryBlock::Create((u8 *)AUDIO_POOL_BASE, AUDIO_POOL_SIZE);

    if (block)
    {
        audioPool.AddBlock(block);
        return true;
    }

    return false;
}

std::pair<void *, size_t> AudioPool::MemoryAlign(size_t size)
{
    if (!audioPool.Ready() && !Initialize())
        return std::pair(nullptr, -1);

    MemoryChunk chunk;
    if (!audioPool.Allocate(chunk, size))
        return std::pair(nullptr, -2);

    return std::pair(chunk.address, chunk.size);
}

void AudioPool::MemoryFree(const std::pair<void *, size_t> & chunk)
{
    audioPool.DeAllocate((u8 *)chunk.first, chunk.second);
}

/* Audio Pool's Memory Pool */

void AudioPool::MemoryPool::CoalesceRight(MemoryBlock * block)
{
    auto current = block->base + block->size;
    auto next = block->next;

    for (auto nxt = next; nxt; nxt = next)
    {
        next = nxt->next;

        if (nxt->base != current)
            break;

        block->size += nxt->size;
        current += nxt->size;

        AudioPool::MemoryPool::DeleteBlock(nxt);
    }
}

bool AudioPool::MemoryPool::Allocate(MemoryChunk & chunk, u64 size)
{
    u64 alignMask = (AUDREN_BUFFER_ALIGNMENT - 1);

    if (size && alignMask)
    {
        if (size > std::numeric_limits<uint32_t>::max())
            return false;

        size = (size + alignMask) &~ alignMask;
    }

    for (auto block = first; block; block = block->next)
    {
        auto address = block->base;

        u64 waste = (u64)address & alignMask;

        if (waste > 0)
            waste = alignMask + 1 - waste;

        if (waste > block->size)
            continue;

        address += waste;

        u64 blockSize = block->size - waste;

        if (blockSize < size)
            continue;

        chunk.address = address;
        chunk.size = size;

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
            auto nextSize = blockSize - size;

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

void AudioPool::MemoryPool::DeAllocate(u8 * chunkAddress, u64 chunkSize)
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
