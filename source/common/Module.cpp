#include "common/Module.hpp"
#include "common/Exception.hpp"

#include <map>
#include <string>
#include <utility>

namespace
{
    using ModuleRegistry = std::map<std::string, love::Module*>;

    ModuleRegistry* registry = nullptr;

    ModuleRegistry& registryInstance()
    {
        if (registry == nullptr)
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

#define W_MODULE_OVERRIDE   "Warning: overwriting module instance {:s} with new instance {:s}\n"
#define W_MODULE_REGISTERED "Module '{:s}' already registered!"

namespace love
{
    Type Module::type("Module", &Object::type);

    Module* Module::instances[] {};

    Module::Module(ModuleType type, const char* name) : moduleType(type), name(name)
    {
        registerInstance(this);
    }

    Module::~Module()
    {
        ModuleRegistry& registry = registryInstance();

        for (auto it = registry.begin(); it != registry.end(); ++it)
        {
            if (it->second == this)
            {
                registry.erase(it);
                break;
            }
        }

        for (int index = 0; index < (int)M_MAX_ENUM; index++)
        {
            if (instances[index] == this)
                instances[index] = nullptr;
        }

        freeEmptyRegistry();
    }

    void Module::registerInstance(Module* instance)
    {
        if (instance == nullptr)
            throw Exception("Module instance is null.");

        std::string name(instance->getName());
        ModuleRegistry& registry = registryInstance();
        auto iterator            = registry.find(name);

        if (iterator != registry.end())
        {
            if (iterator->second == instance)
                return;

            throw Exception(W_MODULE_REGISTERED, instance->getName());
        }

        registry.insert(std::make_pair(name, instance));
        ModuleType type = instance->getModuleType();

        if (type != M_UNKNOWN)
        {
            if (instances[type] != nullptr)
                std::printf(W_MODULE_OVERRIDE, instances[type]->getName(), instance->getName());

            instances[type] = instance;
        }
    }

    Module* Module::getInstance(const std::string& name)
    {
        ModuleRegistry& registry = registryInstance();
        auto iterator            = registry.find(name);

        if (iterator == registry.end())
            return nullptr;

        return iterator->second;
    }
} // namespace love
