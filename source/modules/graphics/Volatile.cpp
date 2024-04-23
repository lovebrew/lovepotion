#include "modules/graphics/Volatile.hpp"

namespace love
{
    std::list<Volatile*> Volatile::all;

    Volatile::Volatile()
    {
        this->all.push_back(this);
    }

    Volatile::~Volatile()
    {
        this->all.remove(this);
    }

    bool Volatile::loadAll()
    {
        bool success = true;
        for (auto* v : Volatile::all)
            success = success && v->loadVolatile();

        return success;
    }

    void Volatile::unloadAll()
    {
        for (auto* v : Volatile::all)
            v->unloadVolatile();
    }
} // namespace love
