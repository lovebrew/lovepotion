#include "common/Exception.hpp"

#include "driver/audio/DigitalSoundMemory.hpp"

#include <cstring>
#include <malloc.h>

namespace love
{
    DigitalSoundMemory::DigitalSoundMemory(DigitalSoundMemory&& other) noexcept :
        base(other.base),
        addressMap(std::move(other.addressMap)),
        pool(std::move(other.pool))
    {
        other.base = nullptr;
    }

    DigitalSoundMemory& DigitalSoundMemory::operator=(DigitalSoundMemory&& other) noexcept
    {
        if (this != &other)
        {
            if (this->base)
            {
                this->pool.destroy();
                std::free(this->base);
            }

            this->base       = other.base;
            this->addressMap = std::move(other.addressMap);
            this->pool       = std::move(other.pool);

            other.base = nullptr;
        }

        return *this;
    }

    DigitalSoundMemory::~DigitalSoundMemory()
    {
        std::lock_guard lock(this->mutex);

        if (this->base)
        {
            this->pool.destroy();
            std::free(this->base);
            this->base = nullptr;
        }
    }

    bool DigitalSoundMemory::initialize(AudioDriver* driver)
    {
        std::lock_guard lock(this->mutex);

        if (this->base)
            return false;

        this->base = memalign(AUDREN_MEMPOOL_ALIGNMENT, this->size);

        if (!this->base)
            return false;

        std::memset(this->base, 0, this->size);

        auto* block = MemoryBlock::create((uint8_t*)this->base, this->size);

        if (block == nullptr)
            return false;

        this->pool.addBlock(block);

        const auto id = audrvMemPoolAdd(driver, this->base, this->size);

        if (id < 0)
            return false;

        bool attached = audrvMemPoolAttach(driver, id);

        if (!attached)
            return false;

        return true;
    }

    void* DigitalSoundMemory::allocate(size_t size)
    {
        std::lock_guard lock(this->mutex);

        if (!this->pool.ready())
        {
            throw love::Exception("Digital sound memory pool is not ready.");
            return nullptr;
        }

        MemoryChunk chunk {};

        if (!this->pool.allocate(chunk, size))
        {
            throw love::Exception("Failed to allocate memory chunk for digital sound.");
            return nullptr;
        }

        auto* node = new AddressNode();

        if (!node)
        {
            this->pool.deallocate(chunk);
            throw love::Exception("Failed to allocate address node for digital sound.");
            return nullptr;
        }

        node->chunk = chunk;
        this->addressMap.insert(node);
        return chunk.address;
    }

    void DigitalSoundMemory::free(void* address)
    {
        std::lock_guard lock(this->mutex);

        if (!address)
            return;

        auto* node = this->addressMap.find(address);

        if (!node)
            return;

        this->pool.deallocate(node->chunk);
        this->addressMap.remove(node);
        delete node;
    }
} // namespace love
