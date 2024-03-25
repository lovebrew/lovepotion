#pragma once

#include "common/Object.hpp"
#include "common/int.hpp"

#include <cstring>
#include <string>
#include <vector>

namespace love
{
    class Variant
    {
      public:
        static constexpr int MAX_SMALL_STRING_LENGTH = 15;

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

        class SharedString : public Object
        {
          public:
            SharedString(const char* string, size_t length) : length(length)
            {
                this->string         = new char[length + 1];
                this->string[length] = '\0';

                std::memcpy(this->string, string, length);
            }

            ~SharedString()
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

            std::vector<std::pair<Variant, Variant>> pairs;
        };

        union Data
        {
            bool boolean;
            double number;
            SharedString* string;
            void* userdata;
            Proxy proxy;
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

        Variant(const char* string, size_t length);

        Variant(const std::string& string);

        Variant(love::Type* type, Object* object);

        Variant(void* lightuserdata);

        Variant(SharedTable* table);

        Variant(const Variant& other);

        Variant(Variant&& other);

        ~Variant();

        Variant& operator=(const Variant& other);

        Type getType() const
        {
            return this->type;
        }

        const Data& getData() const
        {
            return this->data;
        }

        static Variant unknown()
        {
            return Variant(UNKNOWN);
        }

      private:
        Variant(Type type);

        Type type;
        Data data;
    };
} // namespace love
