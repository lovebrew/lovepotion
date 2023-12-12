#include <common/volatile.hpp>

using namespace love;

std::list<Volatile*> Volatile::all;

Volatile::Volatile()
{
    all.push_back(this);
}

Volatile::~Volatile()
{
    all.remove(this);
}

bool Volatile::LoadAll()
{
    bool success = true;
    for (Volatile* v : all)
        success = success && v->LoadVolatile();

    return success;
}

void Volatile::UnloadAll()
{
    for (Volatile* v : all)
        v->UnloadVolatile();
}
