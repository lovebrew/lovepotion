#include <common/type.hpp>

#include <string>

using namespace love;

static std::unordered_map<std::string, Type*> types;

Type::Type(const char* name, Type* parent) :
    name(name),
    parent(parent),
    id(0),
    initialized(false)
{}

void Type::Initialize()
{
    static uint32_t next = 1;

    if (this->initialized)
        return;

    types[this->name] = this;
    this->id          = next++;

    this->bits[this->id] = true;
    this->initialized    = true;

    if (!this->parent)
        return;

    if (!this->parent->initialized)
        this->parent->Initialize();

    this->bits |= this->parent->bits;
}

bool Type::IsA(const Type& other)
{
    if (!this->initialized)
        this->Initialize();

    return this->bits[other.id];
}

bool Type::IsA(const uint32_t& id)
{
    if (!this->initialized)
        this->Initialize();

    return this->bits[id];
}

Type* Type::ByName(const char* name)
{
    auto position = types.find(name);

    if (position == types.end())
        return nullptr;

    return position->second;
}

const char* Type::GetName() const
{
    return this->name;
}
