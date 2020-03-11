#pragma once

#include <string>
#include <vector>

// As StringMap instantiates std::vector<std::string> for instances that use
// getNames(), we end up with multiple copies in the object files. This
// declaration means we only emit it once (in StringMap.cpp).
extern template class std::vector<std::string>;

template <typename T, unsigned int SIZE>
class StringMap
{
    public:
        struct Entry
        {
            const char * key;
            T value;
        };

        StringMap(const Entry * entries, size_t num)
        {
            for (size_t i = 0; i < SIZE; ++i)
                reverse[i] = nullptr;

            size_t count = num / sizeof(Entry);

            for (size_t i = 0; i < count; ++i)
                this->Add(entries[i].key, entries[i].value);
        }

        bool StringEqual(const char * a, const char * b)
        {
            while (*a != 0 && *b != 0)
            {
                if (*a != *b)
                    return false;

                ++a;
                ++b;
            }

            return (*a == 0 && *b == 0);
        }

        bool Find(const char * key, T & value)
        {
            size_t hash = djb2(key);

            for (size_t i = 0; i < MAX; ++i)
            {
                size_t index = (hash + i) % MAX;

                if (!this->records[index].set)
                    return false;

                if (this->StringEqual(this->records[index].key, key))
                {
                    value = this->records[index].value;
                    return true;
                }
            }

            return false;
        }

        bool Find(T key, const char *& string)
        {
            size_t index = (size_t)key;
            if (index >= SIZE)
                return false;

            if (this->reverse[index] != nullptr)
            {
                string = this->reverse[index];
                return true;
            }
            else
                return false;
        }

        bool Add(const char * key, T value)
        {
            size_t hash = djb2(key);
            bool inserted = false;

            for (size_t i = 0; i < MAX; ++i)
            {
                size_t index = (hash + i) % MAX;

                if (!this->records[index].set)
                {
                    inserted = true;

                    this->records[index].set = true;
                    this->records[index].key = key;
                    this->records[index].value = value;

                    break;
                }
            }

            size_t index = (size_t)value;

            if (index >= SIZE)
                return false;

            reverse[index] = key;

            return inserted;
        }

        size_t djb2(const char * key)
        {
            size_t hash = 5381;
            int c;

            while ((c = *key++))
                hash = ((hash << 5) + hash) + c;

            return hash;
        }

        std::vector<std::string> GetNames() const
        {
            std::vector<std::string> names;
            names.reserve(SIZE);

            for (size_t i = 0; i < SIZE; ++i)
            {
                if (this->reverse[i] != nullptr)
                    names.emplace_back(this->reverse[i]);
            }

            return names;
        }

    private:
        struct Record
        {
            const char * key;
            T value;
            bool set;
            Record() : set(false) {}
        };

        static const size_t MAX = SIZE * 2;

        Record records[MAX];
        const char * reverse[SIZE];
};
