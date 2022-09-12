#pragma once

#include <common/luax.hpp>
#include <common/object.hpp>

#include <algorithm>
#include <memory>
#include <set>
#include <variant>
#include <vector>

namespace love
{
    class Variant;

    using VariantPair = std::pair<Variant, Variant>;

    class SharedString : public Object
    {
      public:
        SharedString(const char* string, size_t length)
        {
            this->string = std::make_unique<char[]>(length + 1);
            std::copy_n(string, length, this->string.get());
            this->string[length] = '\0';

            this->length = length;
        }

        std::unique_ptr<char[]> string;
        size_t length;
    };

    struct SmallString
    {
        char string[0x0F];
        uint8_t length;

        SmallString(const char* string, size_t length)
        {
            std::copy_n(string, length, this->string);
            this->length = length;
        }
    };

    class SharedTable : public Object
    {
      public:
        SharedTable()
        {}

        virtual ~SharedTable()
        {}

        std::vector<VariantPair> pairs;
    };

    struct Nil
    {
    };

    // clang-format off
    using variant_t = std::variant<std::monostate, bool, double, Proxy, SharedString*, SharedTable*, SmallString, void*, Nil>;
    // clang-format on

    class Variant
    {
      public:
        enum Type
        {
            UNKNOWN = 0,
            BOOLEAN,
            NUMBER,
            LOVEOBJECT,
            STRING,
            TABLE,
            SMALLSTRING,
            LUSERDATA,
            NIL
        };

        Variant();

        Variant(bool boolean);

        Variant(double number);

        Variant(const char* str, size_t len);

        Variant(const std::string& str);

        Variant(void* lightuserdata);

        Variant(love::Type* type, love::Object* object);

        Variant(SharedTable* table);

        Variant(const Variant& other);

        Variant(Variant&& other);

        ~Variant();

        Variant& operator=(const Variant& v);

        Type getType() const
        {
            return type;
        }

        static Variant Unknown()
        {
            return Variant(UNKNOWN);
        }

        Variant::Type GetType() const
        {
            return this->type;
        };

        std::string GetTypeString() const;

      private:
        variant_t data;
        Variant::Type type;

      public:
        template<Variant::Type type>
        const std::variant_alternative_t<size_t(type), decltype(data)>& GetValue() const
        {
            return std::get<size_t(type)>(data);
        }

        template<Variant::Type type>
        std::variant_alternative_t<size_t(type), decltype(data)>& GetValue()
        {
            return std::get<size_t(type)>(data);
        }

      private:
        Variant(Variant::Type type);
    };
} // namespace love
