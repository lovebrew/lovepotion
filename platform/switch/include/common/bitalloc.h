#pragma once

#include "common/exception.h"
#include <bitset>
#include <deko3d.h>

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

#if defined(__DEBUG__)
        if (!this->test(index))
            throw love::Exception("Texture allocator bit %zu not set!", index);
#endif

        return index;
    }

    void DeAllocate(DkResHandle handle)
    {
        size_t index = this->Find(handle);
        this->reset(index);
    }
};