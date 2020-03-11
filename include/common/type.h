#pragma once

namespace love
{
    class Type
    {
        public:
            static const uint32_t MAX_TYPES = 128;

            void Init();

            Type(const char * name, Type * parent);
            Type(const Type &) = delete;

            const char * GetName() const;

            bool IsA(const love::Type & other);
            bool IsA(const uint32_t & other);

            static Type * ByName(const char * name);

        private:
            const char * const name;
            Type * const parent;

            uint32_t id;
            bool initialized;

            std::bitset<MAX_TYPES> m_bits;

            static inline std::unordered_map<std::string, love::Type *> m_types = {};

    };
}
