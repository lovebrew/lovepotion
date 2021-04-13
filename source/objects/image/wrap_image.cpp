#include "objects/image/wrap_image.h"

using namespace love;

love::Type Image::type("Image", &Texture::type);

Image* Wrap_Image::CheckImage(lua_State* L, int index)
{
    return Luax::CheckType<Image>(L, index);
}

int Wrap_Image::Register(lua_State* L)
{
    return Luax::RegisterType(L, &Image::type, Wrap_Texture::functions, nullptr);
}
