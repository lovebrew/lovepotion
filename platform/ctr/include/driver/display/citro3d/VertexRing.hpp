#pragma once

#include <3ds.h>

namespace love
{
    class VertexRing
    {
      public:
        VertexRing() : memory(nullptr), sliceSize(0), currentSlice(0), sliceCount(0)
        {}

        VertexRing(const VertexRing&) = delete;

        VertexRing& operator=(const VertexRing&) = delete;

        ~VertexRing()
        {
            linearFree(this->memory);
        }

        bool allocate(size_t numSlices, size_t size)
        {
            if (this->memory)
                linearFree(this->memory);

            this->memory = linearAlloc(size * numSlices);

            if (this->memory == nullptr)
                return false;

            this->sliceSize  = size;
            this->sliceCount = numSlices;

            return true;
        }

        size_t getSize()
        {
            return this->sliceSize * this->sliceCount;
        }

        void* getBuffer() const
        {
            return this->memory;
        }

        void* begin() const
        {
            const auto offset = this->currentSlice * this->sliceSize;
            return (void*)((char*)this->memory + offset);
        }

        void end()
        {
            this->currentSlice = (this->currentSlice + 1) % this->sliceCount;
        }

      private:
        void* memory;
        size_t sliceSize;

        size_t currentSlice;
        size_t sliceCount;
    };
} // namespace love
