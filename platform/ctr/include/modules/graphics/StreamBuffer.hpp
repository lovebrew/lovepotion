#pragma once

#include "modules/graphics/StreamBuffer.tcc"

namespace love
{
    StreamBufferBase* createStreamBuffer(BufferUsage usage, size_t size);
}
