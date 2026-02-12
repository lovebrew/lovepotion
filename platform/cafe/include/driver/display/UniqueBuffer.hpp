#pragma once

#include "common/Map.hpp"
#include "modules/graphics/vertex.hpp"

#include <gx2r/buffer.h>

#include <cstring>

namespace love
{
    // clang-format off
    static constexpr auto BUFFER_BASE_FLAGS = GX2R_RESOURCE_USAGE_CPU_READ | GX2R_RESOURCE_USAGE_CPU_WRITE | GX2R_RESOURCE_USAGE_GPU_READ;
    static constexpr auto GX2_BUFFERFLAG_INDEX         = BUFFER_BASE_FLAGS | GX2R_RESOURCE_BIND_INDEX_BUFFER;
    static constexpr auto GX2_BUFFERFLAG_VERTEX        = BUFFER_BASE_FLAGS | GX2R_RESOURCE_BIND_VERTEX_BUFFER;
    static constexpr auto GX2_BUFFERFLAG_UNIFORM_BLOCK = BUFFER_BASE_FLAGS | GX2R_RESOURCE_BIND_UNIFORM_BLOCK;
    // clang-format on

    struct UniqueBuffer
    {
        UniqueBuffer(GX2RBuffer* buffer) :
            buffer(buffer),
            data((uint8_t*)GX2RLockBufferEx(buffer, GX2R_RESOURCE_BIND_NONE))
        {}

        UniqueBuffer(const UniqueBuffer&)            = delete;
        UniqueBuffer& operator=(const UniqueBuffer&) = delete;

        void* fill(size_t size, const void* bytes = nullptr)
        {
            if (bytes == nullptr)
                return std::memset(this->data, 0, size);

            return std::memcpy(this->data, bytes, size);
        }

        ~UniqueBuffer()
        {
            GX2RUnlockBufferEx(this->buffer, GX2R_RESOURCE_BIND_NONE);
        }

        // clang-format off
        ENUMMAP_DECLARE(GX2RBufferFlags, BufferUsage, GX2RResourceFlags,
            { BUFFERUSAGE_INDEX,  GX2_BUFFERFLAG_INDEX  },
            { BUFFERUSAGE_VERTEX, GX2_BUFFERFLAG_VERTEX }
        );
        // clang-format on

      private:
        GX2RBuffer* buffer;
        uint8_t* data;
    };
} // namespace love
