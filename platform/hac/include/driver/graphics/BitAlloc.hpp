#pragma once

#include <bitset>

namespace love
{
    template<size_t N>
    class BitwiseAlloc : std::bitset<N>
    {
      public:
        size_t allocate()
        {
            size_t index = 0;
            while (index < this->size() && this->test(index))
                ++index;

            this->set(index);
            return index;
        }

        void deallocate(uint32_t handle)
        {
            auto index = 0;

            if (this->find(handle, index))
                this->reset(index);
        }

        bool find(uint32_t handle, int& out)
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
} // namespace love
