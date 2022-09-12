#pragma once

#include <bitset>
#include <unordered_map>

#include <stdint.h>

namespace love
{
    class Type
    {
      public:
        static constexpr uint32_t MAX_TYPES = 0x80;

        Type(const char* name, Type* parent);

        Type(const Type&) = delete;

        void Initialize();

        const char* GetName() const;

        bool IsA(const Type& other);

        bool IsA(const uint32_t& id);

        static Type* ByName(const char* name);

      private:
        const char* const name;
        Type* const parent;

        uint32_t id;
        bool initialized;

        std::bitset<MAX_TYPES> bits;
    };
} // namespace love
