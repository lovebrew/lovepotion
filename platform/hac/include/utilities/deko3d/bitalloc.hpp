#pragma once

#include <bitset>

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

    void DeAllocate(uint32_t handle)
    {
        const auto index = this->Find(handle);
        this->reset(index);
    }

    size_t Find(uint32_t handle)
    {
        size_t index = (handle & ((1U << 20) - 1));

        return index;
    }
};
