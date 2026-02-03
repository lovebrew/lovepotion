#pragma once

#include "common/Color.hpp"

#include "modules/graphics/vertex.hpp"

#include <array>
#include <vector>
#define DK_HPP_SUPPORT_VECTOR
#include <deko3d.hpp>

namespace love
{
    namespace vertex
    {
        struct BufferHandle
        {
            CMemPool::Handle memory;
            DkVtxBufferState state;
        };
    } // namespace vertex
} // namespace love
