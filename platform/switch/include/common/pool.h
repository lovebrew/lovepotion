#pragma once

namespace AudioPool
{
    // Initialize BASE during audio module load?
    extern void * AUDIO_POOL_BASE;
    inline u64 AUDIO_POOL_SIZE = 0x1000000;

    struct MemoryChunk
    {
        u8 * address;
        u64 size;
    };

    struct MemoryBlock
    {
        MemoryBlock * prev;
        MemoryBlock * next;

        u8 * base;
        u64 size;

        static MemoryBlock * Create(u8 * base, size_t size)
        {
            MemoryBlock * block = (MemoryBlock *)malloc(sizeof(MemoryBlock));

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
        MemoryBlock * first;
        MemoryBlock * last;

        bool Ready()
        {
            return first != nullptr;
        }

        void AddBlock(MemoryBlock * block)
        {
            block->prev = last;

            if (last)
                last->next = block;

            if (!first)
                first = block;

            last = block;
        }

        void DeleteBlock(MemoryBlock * block)
        {
            auto prev = block->prev;
            auto next = block->next;

            auto &pNext = (prev) ? prev->next : first;
            auto &nPrev = (next) ? next->prev : last;

            pNext = next;
            nPrev = prev;

            free(block);
        }

        void InsertBefore(MemoryBlock * a, MemoryBlock * b)
        {
            auto prev = a->prev;
            auto &pNext = prev ? prev->next : first;

            a->prev = b;
            b->next = a;
            b->prev = prev;

            pNext = b;
        }

        void InsertAfter(MemoryBlock * a, MemoryBlock * b)
        {
            auto next = a->next;
            auto &nPrev = next ? next->prev : last;

            a->next = b;
            b->prev = a;

            b->next = next;

            nPrev = b;
        }

        void CoalesceRight(MemoryBlock * block);

        bool Allocate(MemoryChunk & chunk, size_t size);
        void DeAllocate(u8 * address, size_t size);

        void Destroy()
        {
            MemoryBlock * next = nullptr;

            for (auto block = first; block; block = next)
            {
                next = block->next;
                free(block);
            }

            first = nullptr;
            last = nullptr;
        }
    };

    extern MemoryPool audioPool;

    bool Initialize();

    std::pair<void *, size_t> MemoryAlign(size_t size);

    void MemoryFree(const std::pair<void *, size_t> & chunk);
}
