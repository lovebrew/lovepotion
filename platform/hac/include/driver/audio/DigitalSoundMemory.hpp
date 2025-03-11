#pragma once

#include "driver/audio/MemoryPool.hpp"
#include "driver/display/deko3d/CIntrusiveTree.h"
#include <mutex>

namespace love
{
    struct AddressNode
    {
        CIntrusiveTreeNode node;
        MemoryChunk chunk;

        bool operator<(const AddressNode& other) const
        {
            return this->chunk.address < other.chunk.address;
        }

        bool operator<(const void* address) const
        {
            return this->chunk.address < address;
        }

        friend bool operator<(const void* address, const AddressNode& node)
        {
            return address < node.chunk.address;
        }
    };

    class DigitalSoundMemory
    {
      public:
        DigitalSoundMemory() = default;
        ~DigitalSoundMemory();

        DigitalSoundMemory(const DigitalSoundMemory&)            = delete;
        DigitalSoundMemory& operator=(const DigitalSoundMemory&) = delete;

        DigitalSoundMemory(DigitalSoundMemory&&) noexcept;
        DigitalSoundMemory& operator=(DigitalSoundMemory&&) noexcept;

        void initialize(AudioDriver* driver);

        void* allocate(size_t size);

        void free(void* address);

        void* getBaseAddress()
        {
            return this->base;
        }

        size_t getSize()
        {
            return this->size;
        }

      private:
        void* base                   = nullptr;
        static constexpr size_t size = 0x1000000;
        CIntrusiveTree<AddressNode, &AddressNode::node> addressMap;
        std::mutex mutex;

        MemoryPool pool;
    };
} // namespace love
