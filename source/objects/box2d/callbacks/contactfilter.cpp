#include "callbacks/contactfilter.h"
#include "fixture.h"

using namespace love;

ContactFilter::ContactFilter() : ref(nullptr), L(nullptr)
{}

ContactFilter::~ContactFilter()
{
    if (this->ref != nullptr)
        delete this->ref;
}

bool ContactFilter::Process(Fixture* a, Fixture* b)
{
    int filterA[3], filterB[3];

    a->GetFilterData(filterA);
    b->GetFilterData(filterB);

    if (filterA[2] != 0 &&        // 0 is the default group, so this does not count
        filterA[2] == filterB[2]) // if they are in the same group
        return filterA[2] > 0;    // Negative indexes mean you don't collide

    if ((filterA[1] & filterB[0]) == 0 || (filterB[1] & filterA[0]) == 0)
        return false; // A and B aren't set to collide

    if (ref != nullptr && L != nullptr)
    {
        ref->Push(L);

        Luax::PushType(L, a);
        Luax::PushType(L, b);

        lua_call(L, 2, 1);

        return Luax::CheckBoolean(L, -1);
    }

    return true;
}
