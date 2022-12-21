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
        auto index = 0;

        if (this->Find(handle, index))
            this->reset(index);
    }

    bool Find(uint32_t handle, int& out)
    {
        size_t index = (handle & ((1U << 20) - 1));

        if (this->test(index))
        {
            out = index;
            return true;
        }

        out = -1;
        return false;
    }
};
