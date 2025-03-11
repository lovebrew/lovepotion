#pragma once

#include <stdlib.h>

#include <switch.h>

namespace love
{
    struct MemoryChunk
    {
        uint8_t* address { nullptr };
        size_t size { 0 };
    };

    struct MemoryBlock
    {
        MemoryBlock* prev { nullptr };
        MemoryBlock* next { nullptr };

        uint8_t* base { nullptr };
        size_t size { 0 };

        static MemoryBlock* create(uint8_t* base, size_t size)
        {
            if (!base || size == 0)
                return nullptr;

            auto* block = (MemoryBlock*)malloc(sizeof(MemoryBlock));

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
      public:
        MemoryPool();

        ~MemoryPool();

        MemoryPool(const MemoryPool&)            = delete;
        MemoryPool& operator=(const MemoryPool&) = delete;

        MemoryPool(MemoryPool&& other) noexcept;

        MemoryPool& operator=(MemoryPool&& other) noexcept;

        bool ready()
        {
            return this->first != nullptr;
        }

        void addBlock(MemoryBlock* block);

        void deleteBlock(MemoryBlock* block);

        void insertBefore(MemoryBlock* block, MemoryBlock* newBlock);

        void insertAfter(MemoryBlock* block, MemoryBlock* newBlock);

        void coalesceRight(MemoryBlock* block);

        bool allocate(MemoryChunk& chunk, size_t size);

        void deallocate(const MemoryChunk& chunk);

        void destroy();

      private:
        MemoryBlock* first;
        MemoryBlock* last;
    };
} // namespace love
