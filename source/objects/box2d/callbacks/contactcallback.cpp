#include "callbacks/contactcallback.h"

#include "common/exception.h"
#include "contact/contact.h"
#include "world.h"

using namespace love;

ContactCallback::ContactCallback(World* world) : ref(nullptr), L(nullptr), world(world)
{}

ContactCallback::~ContactCallback()
{
    if (this->ref != nullptr)
        delete this->ref;
}

void ContactCallback::Process(b2Contact* contact, const b2ContactImpulse* impulse)
{
    if (this->ref != nullptr && this->L != nullptr)
    {
        ref->Push(L);

        // Push first fixture.
        {
            Fixture* a = (Fixture*)world->FindObject(contact->GetFixtureA());

            if (a != nullptr)
                Luax::PushType(L, a);
            else
                throw love::Exception("A fixture has escaped Memoizer!");
        }

        // Push second fixture.
        {
            Fixture* b = (Fixture*)world->FindObject(contact->GetFixtureB());
            if (b != nullptr)
                Luax::PushType(L, b);
            else
                throw love::Exception("A fixture has escaped Memoizer!");
        }

        Contact* cobj = (Contact*)world->FindObject(contact);

        if (!cobj)
            cobj = new Contact(world, contact);
        else
            cobj->Retain();

        Luax::PushType(L, cobj);
        cobj->Release();

        int args = 3;
        if (impulse)
        {
            for (int c = 0; c < impulse->count; c++)
            {
                lua_pushnumber(L, Physics::ScaleUp(impulse->normalImpulses[c]));
                lua_pushnumber(L, Physics::ScaleUp(impulse->tangentImpulses[c]));
                args += 2;
            }
        }
        lua_call(L, args, 0);
    }
}
