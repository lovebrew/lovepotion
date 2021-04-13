#include "common/module.h"

#include "common/exception.h"

using namespace love;

namespace
{
    typedef std::map<std::string, Module*> ModuleRegistry;

    ModuleRegistry* registry = nullptr;

    ModuleRegistry& RegistryInstance()
    {
        if (!registry)
            registry = new ModuleRegistry();

        return *registry;
    }

    void freeEmptyRegistry()
    {
        if (registry && registry->empty())
        {
            delete registry;
            registry = nullptr;
        }
    }
} // namespace

// Make our "type" and empty our instances
love::Type Module::type("Module", &Object::type);
Module* Module::instances[] = {};

Module::~Module()
{
    ModuleRegistry& registry = RegistryInstance();

    // Destroy registry
    for (auto it = registry.begin(); it != registry.end(); ++it)
    {
        if (it->second == this)
        {
            registry.erase(it);
            break;
        }
    }

    for (int i = 0; i < (int)M_MAX_ENUM; i++)
    {
        if (instances[i] == this)
            instances[i] = nullptr;
    }

    freeEmptyRegistry();
}

void Module::RegisterInstance(Module* instance)
{
    if (!instance)
        throw love::Exception("Module instance is null.");

    std::string name(instance->GetName());
    ModuleRegistry& registry = RegistryInstance();

    auto it = registry.find(name);

    if (it != registry.end())
    {
        if (it->second == instance)
            return;

        throw love::Exception("Module %s is already registered.", instance->GetName());
    }

    // insert our new registered instance pair
    registry.insert(std::make_pair(name, instance));
    ModuleType otherType = instance->GetModuleType();

    if (instances[otherType] != nullptr)
        printf("Warning: overwriting module instance %s with new instance %s\n",
               instances[otherType]->GetName(), instance->GetName());

    instances[otherType] = instance;
}

Module* Module::GetInstance(const std::string& name)
{
    ModuleRegistry& registry = RegistryInstance();
    auto it                  = registry.find(name);

    if (registry.end() == it)
        return nullptr;

    return it->second;
}
