#include "objects/imagedata/wrap_imagedata.h"

#include "modules/filesystem/filesystem.h"

#include "objects/data/wrap_data.h"
#include "objects/file/file.h"

using namespace love;

ImageData* Wrap_ImageData::CheckImageData(lua_State* L, int index)
{
    return Luax::CheckType<ImageData>(L, index);
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { 0, 0 }
};
// clang-format on

int Wrap_ImageData::Register(lua_State* L)
{
    int result = Luax::RegisterType(L, &ImageData::type, Wrap_Data::functions, functions, nullptr);

    return result;
}
