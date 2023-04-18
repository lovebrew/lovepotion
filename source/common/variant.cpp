#include <common/variant.hpp>

using namespace love;

Variant::Variant(Type vtype) : type(vtype)
{}

Variant::Variant() : type(NIL)
{}

Variant::Variant(bool boolean) : type(BOOLEAN)
{
    this->data.boolean = boolean;
}

Variant::Variant(double number) : type(NUMBER)
{
    this->data.number = number;
}

Variant::Variant(const char* string, size_t len)
{
    if (len <= MAX_SMALL_STRING_LENGTH)
    {
        type = SMALLSTRING;
        std::copy_n(string, len, this->data.smallstring.str);
        this->data.smallstring.len = (uint8_t)len;
    }
    else
    {
        type              = STRING;
        this->data.string = new SharedString(string, len);
    }
}

Variant::Variant(const std::string& str) : Variant(str.c_str(), str.length())
{}

Variant::Variant(void* lightuserdata) : type(LUSERDATA)
{
    this->data.userdata = lightuserdata;
}

Variant::Variant(love::Type* lovetype, love::Object* object) : type(LOVEOBJECT)
{
    this->data.objectproxy.type   = lovetype;
    this->data.objectproxy.object = object;

    if (this->data.objectproxy.object != nullptr)
        this->data.objectproxy.object->Retain();
}

// Variant gets ownership of the vector.
Variant::Variant(SharedTable* table) : type(TABLE)
{
    this->data.table = table;
}

Variant::Variant(const Variant& v) : type(v.type), data(v.data)
{
    if (this->type == STRING)
        this->data.string->Retain();
    else if (this->type == LOVEOBJECT && this->data.objectproxy.object != nullptr)
        this->data.objectproxy.object->Retain();
    else if (this->type == TABLE)
        this->data.table->Retain();
}

Variant::Variant(Variant&& other) : type(std::move(other.type)), data(std::move(other.data))
{
    other.type = NIL;
}

Variant::~Variant()
{
    if (this->type == STRING)
        this->data.string->Release();
    else if (this->type == LOVEOBJECT && this->data.objectproxy.object != nullptr)
        this->data.objectproxy.object->Release();
    else if (this->type == TABLE)
        this->data.table->Release();
}

Variant& Variant::operator=(const Variant& v)
{
    if (v.type == STRING)
        v.data.string->Retain();
    else if (v.type == LOVEOBJECT && v.data.objectproxy.object != nullptr)
        v.data.objectproxy.object->Retain();
    else if (v.type == TABLE)
        v.data.table->Retain();

    if (type == STRING)
        this->data.string->Release();
    else if (type == LOVEOBJECT && this->data.objectproxy.object != nullptr)
        this->data.objectproxy.object->Release();
    else if (type == TABLE)
        this->data.table->Release();

    type = v.type;
    data = v.data;

    return *this;
}
