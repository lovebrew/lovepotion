#pragma once

#include "common/Singleton.tcc"

#include <stdlib.h>

#include <switch.h>

namespace love
{
    struct Chunk
    {
        uint8_t* address;
        size_t size;
    };

    struct Block
    {
        Block* prev;
        Block* next;

        uint8_t* base;
        size_t size;

        static Block* create(uint8_t* base, size_t size)
        {
            auto* block = (Block*)malloc(sizeof(Block));

            if (block == nullptr)
                return nullptr;

            block->prev = nullptr;
            block->next = nullptr;

            block->base = base;
            block->size = size;

            return block;
        }
    };

    struct Pool
    {
        Block* first;
        Block* last;

        bool ready()
        {
            return this->first != nullptr;
        }

        void addBlock(Block* block)
        {
            block->prev = this->last;

            if (this->last != nullptr)
                this->last->next = block;

            if (!this->first)
                this->first = block;

            this->last = block;
        }

        void deleteBlock(Block* block)
        {
            auto* prev   = block->prev;
            auto*& pNext = (prev) ? prev->next : this->first;

            auto* next   = block->next;
            auto*& nNext = (next) ? next->prev : this->last;

            pNext = next;
            nNext = prev;

            std::free(block);
        }

        void insertBefore(Block* block, Block* newBlock)
        {
            auto* prev  = block->prev;
            auto& pNext = (prev) ? prev->next : this->first;

            block->prev    = newBlock;
            newBlock->next = block;

            newBlock->prev = prev;

            pNext = newBlock;
        }

        void insertAfter(Block* block, Block* newBlock)
        {
            auto* next  = block->next;
            auto& nPrev = (next) ? next->prev : this->last;

            block->next    = newBlock;
            newBlock->prev = block;

            newBlock->next = next;

            nPrev = newBlock;
        }

        void coalesceRight(Block* block);

        bool allocate(Chunk& chunk, size_t size);

        void deallocate(const Chunk& chunk);

        void destroy()
        {
            Block* next = nullptr;

            for (auto* block = this->first; block; block = next)
            {
                next = block->next;
                std::free(block);
            }

            this->first = nullptr;
            this->last  = nullptr;
        }
    };
} // namespace love
