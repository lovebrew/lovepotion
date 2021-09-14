#pragma once

#include <bitset>
#include <unordered_map>

#if defined(__3DS__)
    #include <3ds.h>
#elif defined(__SWITCH__)
    #include <switch.h>
#endif

namespace love
{
    class Type
    {
      public:
        static const uint32_t MAX_TYPES = 128;

        Type(const char* name, Type* parent);

        Type(const Type&) = delete;

        void Init();

        const char* GetName() const;

        bool IsA(const love::Type& other);

        bool IsA(const uint32_t& other);

        static Type* ByName(const char* name);

      private:
        const char* const name;
        Type* const parent;

        uint32_t id;
        bool initialized;

        std::bitset<MAX_TYPES> m_bits;

        static inline std::unordered_map<std::string, love::Type*> m_types = {};
    };
} // namespace love
