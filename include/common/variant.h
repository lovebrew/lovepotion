#pragma once

#include "objects/object.h"

namespace love
{
    class Variant
    {
        private:
            std::variant<std::monostate, bool, Proxy, void*, Nil, float, std::string> variant;

        public:
            enum Type
            {
                UNKNOWN = 0,
                BOOLEAN,
                LOVE_OBJECT,
                LUSERDATA,
                NIL,
                NUMBER,
                STRING
            };

            Variant() : variant(std::monostate{}) {}
            Variant(std::monostate v) : variant(v) {}
            Variant(bool v) : variant(v) {}
            Variant(love::Type * type, Object * object);
            Variant(void * v) : variant(v) {}
            Variant(Nil v) : variant(v) {}
            Variant(float v) : variant(v) {}
            Variant(const std::string & v) : variant(v) {}
            Variant(const char * v) : variant(std::string{v}) {}

            Variant & operator= (std::monostate v) { variant = v; return *this; }
            Variant & operator= (bool v) { variant = v; return *this; }
            Variant & operator= (const Proxy & v) { variant = v; return *this; };
            Variant & operator= (void* v) { variant = v; return *this; }
            Variant & operator= (Nil v) { variant = v; return *this; }
            Variant & operator= (float v) { variant = v; return *this; }
            Variant & operator= (const std::string & v) { variant = v; return *this; }
            Variant & operator= (const char * v) { variant = std::string{v}; return *this; }
            Variant & operator= (const Variant & v);

            ~Variant();

            static Proxy * TryExtractProxy(lua_State * L, size_t index);

            Type GetType() const;

            template <Type type>
            const std::variant_alternative_t<size_t(type), decltype(variant)> & GetValue() const
            {
                return std::get<size_t(type)>(variant);
            }

            template <Type type>
            std::variant_alternative_t<size_t(type), decltype(variant)> & GetValue()
            {
                return std::get<size_t(type)>(variant);
            }

            std::string GetTypeString() const;

            static Variant FromLua(lua_State * L, int n);
            void ToLua(lua_State * L) const;
    };
}
