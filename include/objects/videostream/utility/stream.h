#pragma once

#include "objects/object.h"

namespace love
{
    class Stream : public Object
    {
      public:
        static love::Type type;

        virtual ~Stream()
        {}

        virtual void FillBackBuffer()
        {}

        virtual const void* GetFrontBuffer() const = 0;

        virtual size_t GetSize() const = 0;

        virtual bool SwapBuffers() = 0;
    };
} // namespace love
