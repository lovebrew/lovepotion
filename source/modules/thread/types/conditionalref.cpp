#include "modules/thread/types/conditional.h"

using namespace love::thread;

ConditionalRef::ConditionalRef() : conditional(NewConditional())
{}

ConditionalRef::~ConditionalRef()
{
    delete this->conditional;
}

ConditionalRef::operator Conditional*() const
{
    return this->conditional;
}

Conditional* ConditionalRef::operator->() const
{
    return this->conditional;
}