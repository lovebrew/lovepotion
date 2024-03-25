#pragma once

#include <bitset>

#include <stdint.h>

namespace love
{
    class Type
    {
      public:
        static constexpr uint32_t MAX_TYPES = 0x80;

        Type(const char* name, Type* parent);

        Type(const Type&) = delete;

        static Type* byName(const char* name);

        void initialize();

        uint32_t getId();

        const char* getName() const;

        bool isA(const uint32_t& id)
        {
            if (!this->initialized)
                this->initialize();

            return this->bits[id];
        }

        bool isA(const Type& other)
        {
            if (!this->initialized)
                this->initialize();

            return this->bits[other.id];
        }

      private:
        const char* const name;
        Type* const parent;

        uint32_t id;
        bool initialized;

        std::bitset<MAX_TYPES> bits;
    };
} // namespace love
