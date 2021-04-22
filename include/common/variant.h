#pragma once

#include "common/luax.h"

#include "common/exception.h"
#include "objects/object.h"

#include <set>
#include <variant>
#include <vector>

namespace love
{
    class Variant
    {
        static const int MAX_SMALL_STRING_LENGTH = 15;

        struct Nil
        {
        };

        class SharedString : public Object
        {
          public:
            SharedString(const char* string, size_t length) : length(length)
            {
                this->string         = new char[length + 1];
                this->string[length] = '\0';
                memcpy(this->string, string, length);
            }

            virtual ~SharedString()
            {
                delete[] this->string;
            }

            char* string;
            size_t length;
        };

        struct SmallString
        {
            char string[MAX_SMALL_STRING_LENGTH];
            uint8_t length;
        };

        class SharedTable : public Object
        {
          public:
            SharedTable(std::vector<std::pair<Variant, Variant>>* table) : table(table)
            {}

            virtual ~SharedTable()
            {
                delete this->table;
            }

            std::vector<std::pair<Variant, Variant>>* table;
        };

      private:
        std::variant<std::monostate, bool, float, Variant::SharedString*, Variant::SmallString,
                     Variant::SharedTable*, void*, Proxy, Nil>
            variant;

      public:
        enum Type
        {
            UNKNOWN = 0,
            BOOLEAN,
            NUMBER,
            STRING,
            SMALLSTRING,
            TABLE,
            LUSERDATA,
            LOVE_OBJECT,
            NIL
        };

        Variant() : variant(Nil())
        {}

        Variant(std::monostate v) : variant(v)
        {}

        Variant(bool v) : variant(v)
        {}

        Variant(float v) : variant(v)
        {}

        Variant(const std::string& v);

        Variant(const char* v, size_t length);

        Variant(std::vector<std::pair<Variant, Variant>>* table);

        Variant(void* v) : variant(v)
        {}

        Variant(love::Type* type, Object* object);

        Variant(Variant::Nil v) : variant(v)
        {}

        Variant(const Variant& v);

        Variant& operator=(const Variant& v);

        Variant(Variant&& other);

        ~Variant();

        static Proxy* TryExtractProxy(lua_State* L, size_t index);

        Type GetType() const;

        template<Type type>
        const std::variant_alternative_t<size_t(type), decltype(variant)>& GetValue() const
        {
            return std::get<size_t(type)>(variant);
        }

        template<Type type>
        std::variant_alternative_t<size_t(type), decltype(variant)>& GetValue()
        {
            return std::get<size_t(type)>(variant);
        }

        std::string GetTypeString() const;

        static Variant FromLua(lua_State* L, int n, std::set<const void*>* tableSet = nullptr);

        void ToLua(lua_State* L) const;
    };
} // namespace love
