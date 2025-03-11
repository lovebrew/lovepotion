#include "driver/audio/MemoryPool.hpp"

namespace love
{
    MemoryPool::MemoryPool() : first(nullptr), last(nullptr)
    {}

    MemoryPool::~MemoryPool()
    {
        this->destroy();
    }

    MemoryPool::MemoryPool(MemoryPool&& other) noexcept : first(other.first), last(other.last)
    {
        other.first = nullptr;
        other.last  = nullptr;
    }

    MemoryPool& MemoryPool::operator=(MemoryPool&& other) noexcept
    {
        if (this != &other)
        {
            this->destroy();

            this->first = other.first;
            this->last  = other.last;

            other.first = nullptr;
            other.last  = nullptr;
        }

        return *this;
    }

    void MemoryPool::addBlock(MemoryBlock* block)
    {
        if (!block)
            return;

        block->prev = this->last;

        if (this->last)
            this->last->next = block;

        if (!this->first)
            this->first = block;

        this->last = block;
    }

    void MemoryPool::deleteBlock(MemoryBlock* block)
    {
        if (!block)
            return;

        auto* previous = block->prev;
        auto*& pNext   = previous ? previous->next : this->first;

        auto* next   = block->next;
        auto*& nPrev = next ? next->prev : this->last;

        pNext = next;
        nPrev = previous;

        std::free(block);
    }

    void MemoryPool::insertBefore(MemoryBlock* block, MemoryBlock* add)
    {
        if (!block || !add)
            return;

        auto* previous = block->prev;
        auto*& pNext   = previous ? previous->next : this->first;

        block->prev = add;
        add->next   = block;
        add->prev   = previous;
        pNext       = add;
    }

    void MemoryPool::insertAfter(MemoryBlock* block, MemoryBlock* add)
    {
        if (!block || !add)
            return;

        auto* next   = block->next;
        auto*& nPrev = next ? next->prev : this->last;

        block->next = add;
        add->prev   = block;
        add->next   = next;
        nPrev       = add;
    }

    void MemoryPool::destroy()
    {
        MemoryBlock* next = nullptr;
        for (auto* block = this->first; block; block = next)
        {
            next = block->next;
            std::free(block);
        }

        this->first = nullptr;
        this->last  = nullptr;
    }

    void MemoryPool::coalesceRight(MemoryBlock* block)
    {
        if (!block || !block->next)
            return;

        auto* next = block->next;

        if (block->base + block->size == next->base)
        {
            block->size += next->size;
            this->deleteBlock(next);
        }
    }

    bool MemoryPool::allocate(MemoryChunk& chunk, size_t size)
    {
        if (size == 0)
            return false;

        const auto align = (AUDREN_BUFFER_ALIGNMENT - 1);

        if (size > UINT32_MAX - align)
            return false;

        size = (size + align) & ~align;

        for (auto* block = this->first; block; block = block->next)
        {
            auto* address = block->base;
            size_t waste  = (size_t)address & align;

            if (waste > 0)
                waste = (align + 1 - waste);

            if (waste > block->size)
                continue;

            address += waste;
            size_t block_size = block->size - waste;

            if (block_size < size)
                continue;

            chunk.address = address;
            chunk.size    = size;

            if (waste == 0)
            {
                block->base += size;
                block->size -= size;

                if (block->size == 0)
                    this->deleteBlock(block);
            }
            else
            {
                block->size      = waste;
                size_t remaining = block_size - size;

                if (remaining)
                {
                    auto* next = MemoryBlock::create(address + size, remaining);

                    if (next)
                        this->insertAfter(block, next);
                    else
                        chunk.size += remaining;
                }
            }

            return true;
        }

        return false;
    }

    void MemoryPool::deallocate(const MemoryChunk& chunk)
    {
        if (!chunk.address || chunk.size == 0)
            return;

        auto* address = chunk.address;
        auto size     = chunk.size;

        bool done = false;

        MemoryBlock* previous = nullptr;

        for (auto* block = this->first; block; block = block->next)
        {
            auto* base_address = block->base;

            if ((address + size) == base_address)
            {
                block->base = address;
                block->size += size;
                done = true;
            }
            else if ((block->base + block->size) == address)
            {
                block->size += size;
                this->coalesceRight(block);
                done = true;
            }
            else if (address < base_address)
            {
                auto* new_block = MemoryBlock::create(address, size);

                if (!new_block)
                    return;

                if (previous)
                    this->insertAfter(previous, new_block);
                else
                    this->insertBefore(block, new_block);

                done = true;
            }

            previous = block;
        }

        if (!done)
        {
            auto* new_block = MemoryBlock::create(address, size);

            if (!new_block)
                return;

            this->addBlock(new_block);
        }
    }
} // namespace love
