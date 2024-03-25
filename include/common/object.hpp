#pragma once

#include "common/types.hpp"

#include <atomic>

namespace love
{
    class Object
    {
      public:
        static Type type;

        Object();

        Object(const Object& other);

        virtual ~Object() = 0;

        int getReferenceCount() const;

        void retain();

        void release();

      private:
        std::atomic<int> count;
    };

    struct Proxy
    {
        Type* type;
        Object* object;
    };

    enum class Acquire : uint8_t
    {
        RETAIN,
        NO_RETAIN
    };
} // namespace love
