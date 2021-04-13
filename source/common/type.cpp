#include "common/type.h"

using namespace love;

Type::Type(const char* name, Type* parent) : name(name), parent(parent), id(0), initialized(false)
{}

void Type::Init()
{
    static uint32_t nextID = 1;

    if (initialized)
        return;

    m_types[this->name] = this;
    this->id            = nextID++;

    m_bits[this->id] = true;
    initialized      = true;

    if (!this->parent)
        return;

    if (!this->parent->initialized)
        this->parent->Init();

    m_bits |= this->parent->m_bits;
}

bool Type::IsA(const Type& other)
{
    if (!initialized)
        Init();

    return m_bits[other.id];
}

bool Type::IsA(const uint32_t& other)
{
    if (!initialized)
        Init();

    return m_bits[other];
}

Type* Type::ByName(const char* name)
{
    auto position = m_types.find(name);

    if (position == m_types.end())
        return nullptr;

    return position->second;
}

const char* love::Type::GetName() const
{
    return this->name;
}
