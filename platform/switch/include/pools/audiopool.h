#pragma once

#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>

#include <utility>

namespace AudioPool
{
    // Initialize BASE during audio module load?
    extern void* AUDIO_POOL_BASE;
    inline u64 AUDIO_POOL_SIZE = 0x1000000;

    struct MemoryChunk
    {
        u8* address;
        size_t size;
    };

    struct MemoryBlock
    {
        MemoryBlock* prev;
        MemoryBlock* next;

        u8* base;
        size_t size;

        static MemoryBlock* Create(u8* base, size_t size)
        {
            auto block = (MemoryBlock*)malloc(sizeof(MemoryBlock));

            if (!block)
                return nullptr;

            block->prev = nullptr;
            block->next = nullptr;

            block->base = base;
            block->size = size;

            return block;
        }
    };

    struct MemoryPool
    {
        MemoryBlock* first;
        MemoryBlock* last;

        bool Ready()
        {
            return first != nullptr;
        }

        void AddBlock(MemoryBlock* block)
        {
            block->prev = last;

            if (last)
                last->next = block;

            if (!first)
                first = block;

            last = block;
        }

        void DeleteBlock(MemoryBlock* block)
        {
            auto prev   = block->prev;
            auto& pNext = (prev) ? prev->next : first;

            auto next   = block->next;
            auto& nPrev = (next) ? next->prev : last;

            pNext = next;
            nPrev = prev;

            free(block);
        }

        void InsertBefore(MemoryBlock* b, MemoryBlock* p)
        {
            auto prev   = b->prev;
            auto& pNext = (prev) ? prev->next : first;

            b->prev = p;
            p->next = b;

            p->prev = prev;

            pNext = p;
        }

        void InsertAfter(MemoryBlock* b, MemoryBlock* n)
        {
            auto next   = b->next;
            auto& nPrev = (next) ? next->prev : last;

            b->next = n;
            n->prev = b;

            n->next = next;

            nPrev = n;
        }

        void CoalesceRight(MemoryBlock* block);

        bool Allocate(MemoryChunk& chunk, size_t size);
        void DeAllocate(u8* address, size_t size);

        void Destroy()
        {
            MemoryBlock* next = nullptr;

            for (auto block = first; block; block = next)
            {
                next = block->next;
                free(block);
            }

            first = nullptr;
            last  = nullptr;
        }
    };

    extern MemoryPool audioPool;

    bool Initialize();

    std::pair<void*, size_t> MemoryAlign(size_t size);

    void MemoryFree(const std::pair<void*, size_t>& chunk);
} // namespace AudioPool
