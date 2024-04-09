#pragma once

#include "common/Exception.hpp"
#include "common/Singleton.tcc"

#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include <switch.h>

#include <memory>
#include <utility>

namespace love
{
    class DigitalSoundMemory : public Singleton<DigitalSoundMemory>
    {
      private:
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

            void coalesceRight(Block* block)
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

            bool allocate(Chunk& chunk, size_t size)
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

            void deallocate(uint8_t* address, size_t size)
            {
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

        bool initialize()
        {
            if (!this->initMemoryPool())
                return false;

            auto* block = Block::create((uint8_t*)this->base, this->size);

            if (!block)
                return false;

            this->pool.addBlock(block);
            return true;
        }

      public:
        bool initMemoryPool()
        {
            if (!this->base)
                this->base = aligned_alloc(AUDREN_MEMPOOL_ALIGNMENT, size);

            if (!this->base)
                return false;

            return true;
        }

        void* align(size_t size, size_t& alignment)
        {
            if (!this->pool.ready() && !this->initialize())
                return nullptr;

            Chunk chunk {};
            if (!this->pool.allocate(chunk, size))
                return nullptr;

            alignment = chunk.size;
            return chunk.address;
        }

        void free(const void* memory, size_t size)
        {
            this->pool.deallocate((uint8_t*)memory, size);
        }

        void* getBaseAddress()
        {
            return this->base;
        }

        size_t getSize()
        {
            return this->size;
        }

      private:
        Pool pool;

        void* base;
        static constexpr size_t size = 0x1000000;
    };
} // namespace love
