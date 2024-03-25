#include <unordered_map>

#include "common/types.hpp"

namespace love
{
    static std::unordered_map<std::string, Type*> types;

    Type::Type(const char* name, Type* parent) :
        name(name),
        parent(parent),
        id(0),
        initialized(false)
    {}

    void Type::initialize()
    {
        static uint32_t nextId = 1;

        if (this->initialized)
            return;

        types[this->name] = this;

        this->id             = nextId++;
        this->bits[this->id] = true;
        this->initialized    = true;

        if (!this->parent)
            return;

        if (!this->parent->initialized)
            this->parent->initialize();

        this->bits |= this->parent->bits;
    }

    uint32_t Type::getId()
    {
        if (!this->initialized)
            this->initialize();

        return this->id;
    }

    const char* Type::getName() const
    {
        return this->name;
    }

    Type* Type::byName(const char* name)
    {
        auto it = types.find(name);
        if (it == types.end())
            return nullptr;

        return it->second;
    }
} // namespace love
