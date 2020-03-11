#include "common/runtime.h"
#include "modules/data/wrap_datamodule.h"

using namespace love;

#define instance() (Module::GetInstance<DataModule>(Module::M_DATA))

love::data::ContainerType Wrap_DataModule::CheckContainerType(lua_State * L, int index)
{
    const char * string = luaL_checkstring(L, index);
    love::data::ContainerType type = love::data::CONTAINER_STRING;

    if (!DataModule::GetConstant(string, type))
        Luax::EnumError(L, "container type", DataModule::GetConstants(), string);

    return type;
}

int Wrap_DataModule::Register(lua_State * L)
{
    luaL_Reg reg[] =
    {
        { 0, 0 }
    };


    DataModule * instance = instance();

    if (instance == nullptr)
        Luax::CatchException(L, [&]() { instance = new DataModule(); });
    else
        instance->Retain();

    WrappedModule module;

    module.instance = instance;
    module.name = "data";
    module.type = &Module::type;
    module.functions = reg;
    module.types = 0;

    return Luax::RegisterModule(L, module);
}
