#include "common/memory.hpp"

namespace love
{
    size_t alignUp(size_t size, size_t alignment)
    {
        return (size + alignment - 1) & ~(alignment - 1);
    }
} // namespace love
