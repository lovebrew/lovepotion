#include <common/variant.hpp>

using namespace love;

Variant::Variant(Variant::Type type) : type(type)
{
    if (type == Variant::UNKNOWN)
        this->data = std::monostate {};
}

Variant::Variant() : type(NIL)
{
    this->data = Nil {};
}

Variant::Variant(bool boolean) : type(BOOLEAN)
{
    this->data = boolean;
}

Variant::Variant(double number) : type(NUMBER)
{
    this->data = number;
}

Variant::Variant(const char* string, size_t length)
{
    if (length <= 0x0F)
        this->data = SmallString(string, length);
    else
        this->data = new SharedString(string, length);

    this->type = (length <= 0x0F) ? SMALLSTRING : STRING;
}

Variant::Variant(const std::string& string) : Variant(string.c_str(), string.length())
{}

Variant::Variant(void* lightUserdata) : type(LUSERDATA)
{
    this->data = lightUserdata;
}

Variant::Variant(love::Type* type, Object* object) : type(LOVEOBJECT)
{
    this->data = Proxy { .type = type, .object = object };

    if (this->GetValue<Variant::LOVEOBJECT>().object != nullptr)
        this->GetValue<Variant::LOVEOBJECT>().object->Retain();
}

Variant::Variant(SharedTable* table) : type(TABLE)
{
    this->data = table;
}

Variant::Variant(const Variant& other) : data(other.data), type(other.type)
{
    if (this->GetType() == Variant::STRING)
        this->GetValue<Variant::STRING>()->Retain();
    else if (this->GetType() == Variant::LOVEOBJECT && this->GetValue<Variant::LOVEOBJECT>().object)
        this->GetValue<Variant::LOVEOBJECT>().object->Retain();
    else if (this->GetType() == Type::TABLE)
        this->GetValue<Variant::TABLE>()->Retain();
}

Variant::Variant(Variant&& other) : data(std::move(other.data)), type(std::move(other.type))
{
    other.type = Variant::NIL;
}

Variant& Variant::operator=(const Variant& other)
{
    if (other.type == Variant::STRING)
        other.GetValue<Variant::STRING>()->Retain();
    else if (other.type == LOVEOBJECT && this->GetValue<Variant::LOVEOBJECT>().object)
        other.GetValue<Variant::LOVEOBJECT>().object->Retain();
    else if (other.type == Variant::TABLE)
        other.GetValue<Variant::TABLE>()->Retain();

    if (this->type == Variant::STRING)
        this->GetValue<Variant::STRING>()->Release();
    else if (this->type == LOVEOBJECT && this->GetValue<Variant::LOVEOBJECT>().object)
        this->GetValue<Variant::LOVEOBJECT>().object->Release();
    else if (this->type == Variant::TABLE)
        this->GetValue<Variant::TABLE>()->Release();

    this->type = other.type;
    this->data = other.data;

    return *this;
}

Variant::~Variant()
{
    if (this->type == Variant::LOVEOBJECT && this->GetValue<Variant::LOVEOBJECT>().object)
        this->GetValue<Variant::LOVEOBJECT>().object->Release();
    else if (this->type == Variant::STRING)
        this->GetValue<Type::STRING>()->Release();
    else if (this->type == Variant::TABLE)
        this->GetValue<Type::TABLE>()->Release();
}

std::string Variant::GetTypeString() const
{
    switch (this->type)
    {
        case Variant::BOOLEAN:
            return "boolean";
        case Variant::NIL:
            return "nil";
        case Variant::NUMBER:
            return "number";
        case Variant::STRING:
        case Variant::SMALLSTRING:
            return "string";
        case Variant::LOVEOBJECT:
            return "object";
        case Variant::LUSERDATA:
            return "light usersata";
        case Variant::TABLE:
            return "table";
        default:
            break;
    }

    return "unknown";
}
