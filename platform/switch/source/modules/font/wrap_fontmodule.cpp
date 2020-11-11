#include "common/runtime.h"
#include "modules/font/wrap_fontmodule.h"

using namespace love;

#define instance() (Module::GetInstance<FontModule>(Module::M_FONT))

int Wrap_FontModule::Register(lua_State * L)
{
    luaL_reg reg[] = {
        { 0, 0 }
    };

    FontModule * instance = instance();

    if (instance == nullptr)
        Luax::CatchException(L, [&]() { instance = new FontModule(); });
    else
        instance->Retain();

    WrappedModule module;

    module.instance = instance;
    module.name = "font";
    module.functions = reg;
    module.type = &Module::type;
    module.types = nullptr;

    return Luax::RegisterModule(L, module);
}