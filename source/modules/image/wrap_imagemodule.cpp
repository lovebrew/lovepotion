#include "modules/image/wrap_imagemodule.h"
#include "modules/image/imagemodule.h"

using namespace love;

#define instance() (Module::GetInstance<ImageModule>(Module::M_IMAGE))

int Wrap_ImageModule::NewImageData(lua_State* L)
{
    return 0;
}

int Wrap_ImageModule::NewCompressedData(lua_State* L)
{
    return 0;
}

int Wrap_ImageModule::IsCompressed(lua_State* L)
{
    return 0;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "newImageData",      Wrap_ImageModule::NewImageData      },
    { "newCompressedData", Wrap_ImageModule::NewCompressedData },
    { "isCompressed",      Wrap_ImageModule::IsCompressed      },
    { 0,                   0                             }
};

static constexpr lua_CFunction types[] =
{
    0
};
// clang-format on

int Wrap_ImageModule::Register(lua_State* L)
{
    ImageModule* instance = instance();

    if (instance == nullptr)
        Luax::CatchException(L, [&]() { instance = new love::ImageModule(); });
    else
        instance->Retain();

    WrappedModule module;
    module.instance  = instance;
    module.name      = "image";
    module.type      = &Module::type;
    module.functions = functions;
    module.types     = types;

    return Luax::RegisterModule(L, module);
}
