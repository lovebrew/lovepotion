#pragma once

#include <common/type.hpp>

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

        void Retain();

        int GetReferenceCount() const;

        void Release();

      private:
        std::atomic<int> count;
    };
} // namespace love
