#pragma once

#include <common/luax.hpp>
#include <common/object.hpp>

#include <algorithm>
#include <cstring>
#include <set>
#include <vector>

namespace love
{
    class Variant
    {

      public:
        using VariantPair = std::pair<Variant, Variant>;

        static constexpr int MAX_SMALL_STRING_LENGTH = 0x0F;

        class SharedString : public Object
        {
          public:
            SharedString(const char* string, size_t length) : length(length)
            {
                this->string         = new char[length + 1];
                this->string[length] = '\0';

                std::memcpy(this->string, string, length);
            }

            virtual ~SharedString()
            {
                delete[] this->string;
            }

            char* string;
            size_t length;
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

        enum Type
        {
            UNKNOWN = 0,
            BOOLEAN,
            NUMBER,
            STRING,
            SMALLSTRING,
            LUSERDATA,
            LOVEOBJECT,
            NIL,
            TABLE
        };

        union Data
        {
            bool boolean;
            double number;
            SharedString* string;
            void* userdata;
            Proxy objectproxy;
            SharedTable* table;
            struct
            {
                char str[MAX_SMALL_STRING_LENGTH];
                uint8_t len;
            } smallstring;
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

        Variant& operator=(const Variant& other);

        Variant::Type GetType() const
        {
            return this->type;
        };

        const Data& GetData() const
        {
            return this->data;
        }

        static Variant Unknown()
        {
            return Variant(UNKNOWN);
        }

        bool Is(Variant::Type type)
        {
            return this->type == type;
        }

        bool Is(Variant::Type type) const
        {
            return this->type == type;
        }

      private:
        Variant::Type type;
        Data data;

        Variant(Variant::Type type);
    };
} // namespace love
