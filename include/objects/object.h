#pragma once

#include "common/strongref.h"
#include "common/type.h"

#include <atomic>

namespace love
{
    class Object
    {
      public:
        static love::Type type;

        Object();

        Object(const Object& other);

        virtual ~Object() = 0;

        void Retain();

        int GetReferenceCount() const;

        void Release();

      private:
        std::atomic<int> count;
    };
} // namespace love
