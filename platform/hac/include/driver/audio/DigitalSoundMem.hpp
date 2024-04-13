#pragma once

#include "driver/audio/AddressMap.hpp"
#include "driver/audio/MemoryPool.hpp"

namespace love
{
    class DigitalSoundMemory : public Singleton<DigitalSoundMemory>
    {
      public:
        bool initialize()
        {
            if (!this->base)
                this->base = aligned_alloc(AUDREN_MEMPOOL_ALIGNMENT, this->size);

            if (!this->base)
                return false;

            auto* block = Block::create((uint8_t*)this->base, this->size);

            if (!block)
                return false;

            this->pool.addBlock(block);
            addressMap.init(addressNodeComparator);

            return true;
        }

        void* allocate(size_t size)
        {
            if (!this->pool.ready() && !this->initialize())
                return nullptr;

            Chunk chunk {};
            if (!this->pool.allocate(chunk, size))
                return nullptr;

            auto* node = newNode(chunk);

            if (!node)
            {
                this->pool.deallocate(chunk);
                return nullptr;
            }

            if (addressMap.insert(&node->node))
                return chunk.address;

            return nullptr;
        }

        void free(void* address)
        {
            auto* node = getNode(address);

            if (!node)
                return;

            this->pool.deallocate(node->chunk);
            deleteNode(node);
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
        void* base;
        static constexpr size_t size = 0x1000000;
        Pool pool;
    };
} // namespace love
