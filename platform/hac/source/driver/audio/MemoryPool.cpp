#include "driver/audio/MemoryPool.hpp"

namespace love
{
    void Pool::coalesceRight(Block* block)
    {
        auto* current = block->base + block->size;
        auto* next    = block->next;

        for (auto* n = next; n; n = next)
        {
            next = n->next;

            if (n->base != current)
                break;

            block->size += n->size;
            current += n->size;

            this->deleteBlock(n);
        }
    }

    bool Pool::allocate(Chunk& chunk, size_t size)
    {
        size_t alignMask = (AUDREN_BUFFER_ALIGNMENT - 1);

        if (size && alignMask)
        {
            if (size > UINTPTR_MAX - alignMask)
                return false;

            size = (size + alignMask) & ~alignMask;
        }

        for (auto* block = this->first; block; block = block->next)
        {
            auto* address = block->base;

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

                if (block->size == 0)
                    this->deleteBlock(block);
            }
            else
            {
                auto* nextBlock   = address + size;
                uint64_t nextSize = blockSize - size;

                block->size = waste;

                if (nextSize)
                {
                    auto* newBlock = Block::create(nextBlock, nextSize);

                    if (newBlock)
                        this->insertAfter(block, newBlock);
                    else
                        chunk.size += nextSize;
                }
            }

            return true;
        }

        return false;
    }

    void Pool::deallocate(const Chunk& chunk)
    {
        uint8_t* address = chunk.address;
        size_t size      = chunk.size;

        bool done = false;

        for (auto* block = first; !done && block; block = block->next)
        {
            auto* baseAddress = block->base;

            if (baseAddress > address)
            {
                if ((address + size) == baseAddress)
                {
                    block->base = address;
                    block->size += size;
                }
                else
                {
                    auto* chunk = Block::create(address, size);

                    if (chunk)
                        this->insertBefore(block, chunk);
                }
                done = true;
            }
            else if ((block->base + block->size) == address)
            {
                block->size += size;
                this->coalesceRight(block);
                done = true;
            }
        }

        if (!done)
        {
            auto* block = Block::create(address, size);

            if (block)
                this->addBlock(block);
        }
    }
} // namespace love
