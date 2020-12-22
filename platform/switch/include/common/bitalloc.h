#pragma once

#include <deko3d.h>
#include <bitset>
#include "common/exception.h"

template<size_t N>
class BitwiseAlloc : std::bitset<N>
{
    public:
        size_t Allocate()
        {
            size_t index = 0;
            while (index < this->size() && this->test(index))
                ++index;

            this->set(index);
            return index;
        }

        size_t Find(DkResHandle handle)
        {
            size_t index = (handle & ((1U << 20) - 1));

            #if defined (__DEBUG__)
                if (!this->test(index))
                    throw love::Exception("Texture allocator bit not set!");
            #endif

            return index;
        }

        void DeAllocate(size_t index)
        {
            #if defined (__DEBUG__)
                if (!this->test(index))
                    throw love::Exception("Texture allocator bit not set!");
            #endif

            this->reset(index);
        }

        void DeAllocate(DkResHandle handle)
        {
            size_t index = this->Find(handle);

            #if defined (__DEBUG__)
                if (!this->test(index))
                    throw love::Exception("Texture allocator bit not set!");
            #endif

            this->reset(index);
        }
};