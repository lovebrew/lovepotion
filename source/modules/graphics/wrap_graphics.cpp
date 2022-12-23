#include <modules/graphics/wrap_graphics.hpp>

#include <modules/graphics_ext.hpp>
#include <modules/image/imagemodule.hpp>

#include <modules/filesystem/wrap_filesystem.hpp>

#include <objects/font/wrap_font.hpp>
#include <objects/imagedata/wrap_imagedata.hpp>
#include <objects/quad/wrap_quad.hpp>
#include <objects/rasterizer/wrap_rasterizer.hpp>
#include <objects/texture/wrap_texture.hpp>

#include <utilities/driver/renderer_ext.hpp>

using Renderer = love::Renderer<love::Console::Which>;

using namespace love;
#define instance() (Module::GetInstance<Graphics<Console::Which>>(Module::M_GRAPHICS))

#if !defined(__3DS__)
std::span<const luaL_Reg> Wrap_Graphics::extensions;
#endif

static int checkGraphicsCreated(lua_State* L)
{
    if (!instance()->IsCreated())
        return luaL_error(L, "love.graphics cannot function without a window!");

    return 0;
}

int Wrap_Graphics::Reset(lua_State*)
{
    instance()->Reset();
    return 0;
}

int Wrap_Graphics::Clear(lua_State* L)
{
    OptionalColor color(Color {});
    std::vector<OptionalColor> colors;

    OptionalInt stencil(0);
    OptionalDouble depth(1.0);

    int argtype = lua_type(L, 1);
    int start   = -1;

    if (argtype == LUA_TTABLE)
    {
        int maxValues = lua_gettop(L);
        colors.reserve(maxValues);

        for (int index = 0; index < maxValues; index++)
        {
            argtype = lua_type(L, index + 1);

            if (argtype == LUA_TNUMBER || argtype == LUA_TBOOLEAN)
            {
                start = (index + 1);
                break;
            }
            else if (argtype == LUA_TNIL || argtype == LUA_TNONE)
            {
                colors.push_back(OptionalColor(Color {}));
                continue;
            }

            for (int colorIndex = 1; colorIndex <= 4; colorIndex++)
                lua_rawgeti(L, (index + 1), colorIndex);

            OptionalColor newColor {};

            newColor.value().r = luaL_checknumber(L, 1);
            newColor.value().g = luaL_checknumber(L, 2);
            newColor.value().b = luaL_checknumber(L, 3);
            newColor.value().a = luaL_optnumber(L, 4, 1.0);

            colors.push_back(newColor);

            lua_pop(L, 4);
        }
    }
    else if (argtype == LUA_TBOOLEAN)
        start = 2;
    else if (argtype != LUA_TNONE && argtype != LUA_TNIL)
    {
        color.value().r = luaL_checknumber(L, 1);
        color.value().g = luaL_checknumber(L, 2);
        color.value().b = luaL_checknumber(L, 3);
        color.value().a = luaL_optnumber(L, 4, 1.0);

        start = 5;
    }

    if (start >= 0)
    {
        argtype = lua_type(L, start);

        if (argtype == LUA_TBOOLEAN)
        {}
        else if (argtype == LUA_TNUMBER)
            stencil.value() = luaL_checkinteger(L, start);

        argtype = lua_type(L, start + 1);

        if (argtype == LUA_TBOOLEAN)
        {}
        else if (argtype == LUA_TNUMBER)
            depth.value() = luaL_checknumber(L, start + 1);
    }

    if (colors.empty())
        luax::CatchException(L, [&]() { instance()->Clear(color, stencil, depth); });
    else
        luax::CatchException(L, [&]() { instance()->Clear(colors, stencil, depth); });

    return 0;
}

int Wrap_Graphics::SetBackgroundColor(lua_State* L)
{
    Color color {};

    if (lua_istable(L, 1))
    {
        for (int i = 1; i <= 4; i++)
            lua_rawgeti(L, 1, i);

        color.r = luaL_checknumber(L, -4);
        color.g = luaL_checknumber(L, -3);
        color.b = luaL_checknumber(L, -2);
        color.a = luaL_optnumber(L, -1, 1.0f);

        lua_pop(L, 4);
    }
    else if (lua_isnumber(L, 1))
    {
        color.r = luaL_checknumber(L, 1);
        color.g = luaL_checknumber(L, 2);
        color.b = luaL_checknumber(L, 3);
        color.a = luaL_optnumber(L, 4, 1.0f);
    }

    instance()->SetBackgroundColor(color);

    return 0;
}

int Wrap_Graphics::GetBackgroundColor(lua_State* L)
{
    const auto color = instance()->GetBackgroundColor();

    lua_pushnumber(L, color.r);
    lua_pushnumber(L, color.g);
    lua_pushnumber(L, color.b);
    lua_pushnumber(L, color.a);

    return 4;
}

int Wrap_Graphics::SetColor(lua_State* L)
{
    Color color {};

    if (lua_istable(L, 1))
    {
        for (int i = 1; i <= 4; i++)
            lua_rawgeti(L, 1, i);

        color.r = luaL_checknumber(L, -4);
        color.g = luaL_checknumber(L, -3);
        color.b = luaL_checknumber(L, -2);
        color.a = luaL_optnumber(L, -1, 1.0f);

        lua_pop(L, 4);
    }
    else if (lua_isnumber(L, 1))
    {
        color.r = luaL_checknumber(L, 1);
        color.g = luaL_checknumber(L, 2);
        color.b = luaL_checknumber(L, 3);
        color.a = luaL_optnumber(L, 4, 1.0f);
    }

    instance()->SetColor(color);

    return 0;
}

int Wrap_Graphics::GetColor(lua_State* L)
{
    const auto color = instance()->GetColor();

    lua_pushnumber(L, color.r);
    lua_pushnumber(L, color.g);
    lua_pushnumber(L, color.b);
    lua_pushnumber(L, color.a);

    return 4;
}

int Wrap_Graphics::GetWidth(lua_State* L)
{
    const char* screenName = luaL_optstring(L, 1, Graphics<Console::Which>::DEFAULT_SCREEN);
    std::optional<Screen> screen;

    if (!(screen = ::Renderer::Instance().CheckScreen(screenName)))
        return luax::EnumError(L, "screen name", ::Renderer::Instance().GetScreens(), screenName);

    lua_pushinteger(L, instance()->GetWidth(*screen));

    return 1;
}

int Wrap_Graphics::GetHeight(lua_State* L)
{
    lua_pushinteger(L, instance()->GetHeight());

    return 1;
}

int Wrap_Graphics::GetDimensions(lua_State* L)
{
    const char* screenName = luaL_optstring(L, 1, Graphics<Console::Which>::DEFAULT_SCREEN);
    std::optional<Screen> screen;

    if (!(screen = ::Renderer::Instance().CheckScreen(screenName)))
        return luax::EnumError(L, "screen name", ::Renderer::Instance().GetScreens(), screenName);

    lua_pushinteger(L, instance()->GetWidth(*screen));
    lua_pushinteger(L, instance()->GetHeight());

    return 2;
}

int Wrap_Graphics::IsActive(lua_State* L)
{
    luax::PushBoolean(L, instance()->IsActive());

    return 1;
}

int Wrap_Graphics::IsCreated(lua_State* L)
{
    luax::PushBoolean(L, instance()->IsCreated());

    return 1;
}

int Wrap_Graphics::SetActiveScreen(lua_State* L)
{
    const char* name = luaL_checkstring(L, 1);
    std::optional<Screen> screen;

    if (screen = ::Renderer::Instance().CheckScreen(name); !screen)
        return luax::EnumError(L, "screen name", ::Renderer::Instance().GetScreens(), name);

    instance()->SetActiveScreen(*screen);

    return 0;
}

int Wrap_Graphics::GetScreens(lua_State* L)
{
    const auto screens = ::Renderer::Instance().GetScreens();

    lua_createtable(L, screens.size(), 0);

    for (size_t index = 0; index < screens.size(); index++)
    {
        luax::PushString(L, screens[index]);
        lua_rawseti(L, -2, (index + 1));
    }

    return 1;
}

int Wrap_Graphics::Origin(lua_State* L)
{
    instance()->Origin();

    return 0;
}

int Wrap_Graphics::Present(lua_State* L)
{
    luax::CatchException(L, [&]() { instance()->Present(); });

    return 0;
}

int Wrap_Graphics::NewFont(lua_State* L)
{
    checkGraphicsCreated(L);

    Font<Console::Which>* font = nullptr;
    if (!luax::IsType(L, 1, Rasterizer<Console::Which>::type))
    {
        std::vector<int> indices;
        for (int i = 0; i < lua_gettop(L); i++)
            indices.push_back(i + 1);

        luax::ConvertObject(L, indices, "font", "newRasterizer");
    }

    auto* rasterizer = Wrap_Rasterizer::CheckRasterizer(L, 1);

    luax::CatchException(L, [&]() { font = instance()->NewFont(rasterizer); });

    luax::PushType(L, font);
    font->Release();

    return 1;
}

int Wrap_Graphics::SetFont(lua_State* L)
{
    auto* font = Wrap_Font::CheckFont(L, 1);
    instance()->SetFont(font);

    return 0;
}

int Wrap_Graphics::GetFont(lua_State* L)
{
    Font<Console::Which>* font = nullptr;
    luax::CatchException(L, [&]() { font = instance()->GetFont(); });

    luax::PushType(L, font);

    return 1;
}

int Wrap_Graphics::Print(lua_State* L)
{
    Font<>::ColoredStrings strings {};
    Wrap_Font::CheckColoredString(L, 1, strings);

    if (luax::IsType(L, 2, Font<>::type))
    {
        auto* font = Wrap_Font::CheckFont(L, 2);

        Wrap_Graphics::CheckStandardTransform(L, 3, [&](const Matrix4<Console::Which>& matrix) {
            luax::CatchException(L, [&]() { instance()->Print(strings, font, matrix); });
        });
    }
    else
    {
        Wrap_Graphics::CheckStandardTransform(L, 2, [&](const Matrix4<Console::Which>& matrix) {
            luax::CatchException(L, [&]() { instance()->Print(strings, matrix); });
        });
    }

    return 0;
}

int Wrap_Graphics::Printf(lua_State* L)
{
    Font<>::ColoredStrings strings {};
    Wrap_Font::CheckColoredString(L, 1, strings);

    Font<Console::Which>* font = nullptr;
    int start                  = 2;

    if (luax::IsType(L, start, Font<>::type))
    {
        font = Wrap_Font::CheckFont(L, start);
        start++;
    }

    Matrix4<Console::Which> matrix;

    int formatIndex = start + 2;

    /* todo check for Transform objects */
    // if (luax::IsType(L, start, love::Transform<love::Console::Which>::type))
    // {
    //     love::Transform<Console::Which>* tf = luax::ToType<Transform<Console::Which>>(L,
    //     start); func(tf->getMatrix());
    // }
    // else
    {
        float x  = luaL_optnumber(L, start + 0, 0.0);
        float y  = luaL_optnumber(L, start + 1, 0.0);
        float a  = luaL_optnumber(L, start + 4, 0.0);
        float sx = luaL_optnumber(L, start + 5, 1.0);
        float sy = luaL_optnumber(L, start + 6, sx);
        float ox = luaL_optnumber(L, start + 7, 0.0);
        float oy = luaL_optnumber(L, start + 8, 0.0);
        float kx = luaL_optnumber(L, start + 9, 0.0);
        float ky = luaL_optnumber(L, start + 10, 0.0);

        matrix = Matrix4<Console::Which>(x, y, a, sx, sy, ox, oy, kx, ky);
    }

    float wrap            = luaL_checknumber(L, formatIndex);
    const char* alignment = luaL_checkstring(L, formatIndex + 1);

    std::optional<Font<>::AlignMode> align;

    if (!(align = Font<>::alignModes.Find(alignment)))
        return luax::EnumError(L, "alignment", Font<>::alignModes.GetNames(), alignment);

    if (font != nullptr)
        luax::CatchException(L, [&]() { instance()->Printf(strings, font, wrap, *align, matrix); });
    else
        luax::CatchException(L, [&]() { instance()->Printf(strings, wrap, *align, matrix); });

    return 0;
}

static void checkTextureSettings(lua_State* L, int index, bool option, bool checkType,
                                 bool checkDimensions, std::optional<bool> forceRenderTarget,
                                 Texture<>::Settings& settings, bool& setDpiScale)
{
    setDpiScale = false;

    if (forceRenderTarget.has_value())
        settings.renderTarget = forceRenderTarget.value();

    if (option && lua_isnoneornil(L, index))
        return;

    luax::CheckTableFields<Texture<>::SettingType>(
        L, index, "texture setting name", [](const char* v) {
            std::optional<::Texture<>::SettingType> value;
            return (Texture<>::settingsTypes.Find(v) != std::nullopt);
        });

    if (!forceRenderTarget.has_value())
    {
        bool defaultValue = settings.renderTarget;
        const char* key   = *Texture<>::settingsTypes.ReverseFind(Texture<>::SETTING_RENDER_TARGET);

        settings.renderTarget = luax::BoolFlag(L, index, key, defaultValue);
    }

    lua_getfield(L, index, *Texture<>::settingsTypes.ReverseFind(Texture<>::SETTING_FORMAT));
    if (!lua_isnoneornil(L, -1))
    {
        const char* string = luaL_checkstring(L, -1);
        std::optional<PixelFormat> format;

        if (!(format = pixelFormats.Find(string)))
            luax::EnumError(L, "pixel format", string);
    }
    lua_pop(L, 1);

    if (checkType)
    {
        lua_getfield(L, index, *Texture<>::settingsTypes.ReverseFind(Texture<>::SETTING_TYPE));
        if (!lua_isnoneornil(L, -1))
        {
            const char* string = luaL_checkstring(L, -1);
            std::optional<Texture<>::TextureType> type;

            if (!(type = Texture<>::textureTypes.Find(string)))
                luax::EnumError(L, "texture type", string);
        }
    }

    if (checkDimensions)
    {
        const char* keyWidth  = *Texture<>::settingsTypes.ReverseFind(Texture<>::SETTING_WIDTH);
        const char* keyHeight = *Texture<>::settingsTypes.ReverseFind(Texture<>::SETTING_HEIGHT);

        settings.width  = luax::CheckIntFlag(L, index, keyWidth);
        settings.height = luax::CheckIntFlag(L, index, keyHeight);

        if (settings.type == Texture<>::TEXTURE_2D_ARRAY ||
            settings.type == Texture<>::TEXTURE_VOLUME)
        {
            const char* keyLayers =
                *Texture<>::settingsTypes.ReverseFind(Texture<>::SETTING_LAYERS);

            settings.layers = luax::CheckIntFlag(L, index, keyLayers);
        }
    }

    lua_getfield(L, index, *Texture<>::settingsTypes.ReverseFind(Texture<>::SETTING_MIPMAPS));
    if (!lua_isnoneornil(L, -1))
    {
        if (lua_type(L, -1) == LUA_TBOOLEAN)
        {
            const auto hasMipmaps = luax::ToBoolean(L, -1);
            settings.mipmaps = hasMipmaps ? Texture<>::MIPMAPS_MANUAL : Texture<>::MIPMAPS_NONE;
        }
        else
        {
            const char* string = luaL_checkstring(L, -1);
            std::optional<Texture<>::MipmapsMode> mode;

            if (!(mode = Texture<>::mipmapModes.Find(string)))
                luax::EnumError(L, "Texture mipmap mode", Texture<>::mipmapModes, string);
        }
    }
    lua_pop(L, 1);

    const char* keyLinear = *Texture<>::settingsTypes.ReverseFind(Texture<>::SETTING_LINEAR);
    const char* keyMSAA   = *Texture<>::settingsTypes.ReverseFind(Texture<>::SETTING_MSAA);

    settings.linear = luax::BoolFlag(L, index, keyLinear, settings.linear);
    settings.msaa   = luax::IntFlag(L, index, keyMSAA, settings.msaa);

    const char* keyComputeWrite =
        *Texture<>::settingsTypes.ReverseFind(Texture<>::SETTING_COMPUTE_WRITE);

    settings.computeWrite = luax::BoolFlag(L, index, keyComputeWrite, settings.computeWrite);

    lua_getfield(L, index, *Texture<>::settingsTypes.ReverseFind(Texture<>::SETTING_READABLE));
    if (!lua_isnoneornil(L, -1))
        settings.readable = luax::CheckBoolean(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, index, *Texture<>::settingsTypes.ReverseFind(Texture<>::SETTING_DPI_SCALE));
    if (lua_isnumber(L, -1))
    {
        settings.dpiScale = lua_tonumber(L, -1);
        setDpiScale       = true;
    }
    lua_pop(L, 1);
}

static void parseDPIScale(Data* fileData, float* dpiScale)
{
    auto* data = (FileData*)fileData;

    if (data == nullptr)
        return;

    const std::string& filename = data->GetName();

    size_t nameLength = filename.length();
    size_t position   = filename.rfind('@');
}

static std::pair<StrongReference<ImageData<Console::Which>>, StrongReference<CompressedImageData>>
getImageData(lua_State* L, int index, bool allowCompressed, float* dpiScale)
{
    StrongReference<ImageData<Console::Which>> image;
    StrongReference<CompressedImageData> compressed;

    if (luax::IsType(L, index, ImageData<>::type))
        image.Set(Wrap_ImageData::CheckImageData(L, index));
    else if (luax::IsType(L, index, CompressedImageData::type))
    {
        /* todo */
    }
    else if (Wrap_Filesystem::CanGetData(L, index))
    {
        auto* module = Module::GetInstance<ImageModule>(Module::M_IMAGE);

        if (module == nullptr)
            luaL_error(L, "Cannot load images without the image module.");

        StrongReference<Data> data(Wrap_Filesystem::GetFileData(L, index), Acquire::NORETAIN);

        if (dpiScale != nullptr)
            parseDPIScale(data, dpiScale);

        if (allowCompressed && module->IsCompressed(data))
        {
            luax::CatchException(L, [&]() {
                /* todo */
                // compressed.Set(module->NewCompressedData(fileData), Acquire::NORETAIN);
            });
        }
        else
        {
            luax::CatchException(
                L, [&]() { image.Set(module->NewImageData(data), Acquire::NORETAIN); });
        }
    }
    else
        image.Set(Wrap_ImageData::CheckImageData(L, index));

    return std::make_pair(image, compressed);
}

static int pushNewTexture(lua_State* L, Texture<>::Slices* slices,
                          const Texture<>::Settings& settings)
{
    StrongReference<Texture<Console::Which>> image;

    // clang-format off
    luax::CatchException(L,
        [&]() { image.Set(instance()->NewTexture(settings, slices), Acquire::NORETAIN); },
        [&](bool) { if (slices) slices->Clear(); }
    );
    // clang-format on

    luax::PushType(L, image);

    return 1;
}

int Wrap_Graphics::Draw(lua_State* L)
{
    Drawable* drawable               = nullptr;
    Texture<Console::Which>* texture = nullptr;
    Quad* quad                       = nullptr;

    int start = 2;

    if (luax::IsType(L, 2, Quad::type))
    {
        texture = Wrap_Texture::CheckTexture(L, 1);
        quad    = luax::ToType<Quad>(L, 2);

        start = 3;
    }
    else if (lua_isnil(L, 2) && !lua_isnoneornil(L, 3))
        return luax::TypeError(L, 2, "Quad");
    else
    {
        drawable = luax::CheckType<Drawable>(L, 1);
        start    = 2;
    }

    // clang-format off
    Wrap_Graphics::CheckStandardTransform(L, start, [&](const Matrix4<Console::Which>& matrix) {
        luax::CatchException(L, [&](){
            if (texture && quad)
                instance()->Draw(texture, quad, matrix);
            else
                instance()->Draw(drawable, matrix);
        });
    });
    // clang-format on

    return 0;
}

int Wrap_Graphics::NewTexture(lua_State* L)
{
    checkGraphicsCreated(L);

    Texture<>::Slices slices(Texture<>::TEXTURE_2D);
    Texture<>::Slices* slicesReference = &slices;

    Texture<>::Settings settings {};
    settings.type = Texture<>::TEXTURE_2D;
    bool dpiSet   = false;

    if (lua_type(L, 1) == LUA_TNUMBER)
    {
        slicesReference = nullptr;

        settings.width  = luaL_checkinteger(L, 1);
        settings.height = luaL_checkinteger(L, 2);

        int start = 3;
        if (lua_type(L, 3) == LUA_TNUMBER)
        {
            settings.layers = luaL_checkinteger(L, 3);
            settings.type   = Texture<>::TEXTURE_2D_ARRAY;
            start           = 4;
        }

        checkTextureSettings(L, start, true, true, false, {}, settings, dpiSet);
    }
    else
    {
        checkTextureSettings(L, 2, true, false, false, {}, settings, dpiSet);
        float* autoDpiScale = dpiSet ? nullptr : &settings.dpiScale;

        if (lua_istable(L, 1))
        {
            int count = std::max<int>(1, luax::ObjectLength(L, 1));

            for (int index = 0; index < count; index++)
            {
                lua_rawgeti(L, 1, index + 1);
                auto data = getImageData(L, -1, true, index == 0 ? autoDpiScale : nullptr);

                if (data.first.Get())
                    slices.Set(0, index, data.first);
                else
                    slices.Set(0, index, data.second->GetSlice(0, 0));
            }
            lua_pop(L, count);
        }
        else
        {
            auto data = getImageData(L, 1, true, autoDpiScale);

            if (data.first.Get())
                slices.Set(0, 0, data.first);
            else
                slices.Add(data.second, 0, 0, false, settings.mipmaps != Texture<>::MIPMAPS_NONE);
        }
    }

    return pushNewTexture(L, slicesReference, settings);
}

int Wrap_Graphics::Push(lua_State* L)
{
    std::optional<Graphics<>::StackType> stackType;
    const char* name = luaL_optstring(L, 1, "transform");

    if (!(stackType = Graphics<>::stackTypes.Find(name)))
        return luax::EnumError(L, "graphics stack type", Graphics<>::stackTypes, name);

    luax::CatchException(L, [&]() { instance()->Push(*stackType); });

    if (luax::IsType(L, 2, Transform::type))
    {
        auto* transform = luax::ToType<Transform>(L, 2);
        luax::CatchException(L, [&]() { instance()->ApplyTransform(transform->GetMatrix()); });
    }

    return 0;
}

int Wrap_Graphics::Translate(lua_State* L)
{
    float x = luaL_checknumber(L, 1);
    float y = luaL_checknumber(L, 2);

    instance()->Translate(x, y);

    return 0;
}

int Wrap_Graphics::Scale(lua_State* L)
{
    float x = luaL_checknumber(L, 1);
    float y = luaL_checknumber(L, 2);

    instance()->Scale(x, y);

    return 0;
}

int Wrap_Graphics::Rotate(lua_State* L)
{
    float angle = luaL_checknumber(L, 1);

    instance()->Rotate(angle);

    return 0;
}

int Wrap_Graphics::Shear(lua_State* L)
{
    float x = luaL_checknumber(L, 1);
    float y = luaL_checknumber(L, 2);

    instance()->Shear(x, y);

    return 0;
}

int Wrap_Graphics::ApplyTransform(lua_State* L)
{
    Wrap_Graphics::CheckStandardTransform(L, 1, [&](const Matrix4<Console::Which>& matrix) {
        luax::CatchException(L, [&]() { instance()->ApplyTransform(matrix); });
    });

    return 0;
}

int Wrap_Graphics::ReplaceTransform(lua_State* L)
{
    Wrap_Graphics::CheckStandardTransform(L, 1, [&](const Matrix4<Console::Which>& matrix) {
        luax::CatchException(L, [&]() { instance()->ReplaceTransform(matrix); });
    });

    return 0;
}

int Wrap_Graphics::TransformPoint(lua_State* L)
{
    Vector2 point {};
    point.x = luaL_checknumber(L, 1);
    point.y = luaL_checknumber(L, 2);

    point = instance()->TransformPoint(point);

    lua_pushnumber(L, point.x);
    lua_pushnumber(L, point.y);

    return 2;
}

int Wrap_Graphics::InverseTransformPoint(lua_State* L)
{
    Vector2 point {};
    point.x = luaL_checknumber(L, 1);
    point.y = luaL_checknumber(L, 2);

    point = instance()->InverseTransformPoint(point);

    lua_pushnumber(L, point.x);
    lua_pushnumber(L, point.y);

    return 2;
}

int Wrap_Graphics::Pop(lua_State* L)
{
    luax::CatchException(L, [&]() { instance()->Pop(); });

    return 0;
}

int Wrap_Graphics::NewQuad(lua_State* L)
{
    checkGraphicsCreated(L);

    Quad::Viewport viewport {};

    viewport.x = luaL_checknumber(L, 1);
    viewport.y = luaL_checknumber(L, 2);
    viewport.w = luaL_checknumber(L, 3);
    viewport.h = luaL_checknumber(L, 4);

    double sourceWidth  = 0.0;
    double sourceHeight = 0.0;
    int layer           = 0;

    if (luax::IsType(L, 5, Texture<>::type))
    {
        auto* texture = Wrap_Texture::CheckTexture(L, 5);

        sourceWidth  = texture->GetWidth();
        sourceHeight = texture->GetHeight();
    }
    else if (luax::IsType(L, 6, Texture<>::type))
    {
        layer         = luaL_checkinteger(L, 5) - 1;
        auto* texture = Wrap_Texture::CheckTexture(L, 6);

        sourceWidth  = texture->GetWidth();
        sourceHeight = texture->GetHeight();
    }
    else if (!lua_isnoneornil(L, 7))
    {
        layer = luaL_checkinteger(L, 5) - 1;

        sourceWidth  = luaL_checknumber(L, 6);
        sourceHeight = luaL_checknumber(L, 7);
    }
    else
    {
        sourceWidth  = luaL_checknumber(L, 5);
        sourceHeight = luaL_checknumber(L, 6);
    }

    auto* quad = instance()->NewQuad(viewport, sourceWidth, sourceHeight);
    quad->SetLayer(layer);

    luax::PushType(L, quad);
    quad->Release();

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "applyTransform",        Wrap_Graphics::ApplyTransform        },
    { "clear",                 Wrap_Graphics::Clear                 },
    { "draw",                  Wrap_Graphics::Draw                  },
    { "isActive",              Wrap_Graphics::IsActive              },
    { "isCreated",             Wrap_Graphics::IsCreated             },
    { "inverseTransformPoint", Wrap_Graphics::InverseTransformPoint },
    { "origin",                Wrap_Graphics::Origin                },
    { "pop",                   Wrap_Graphics::Pop                   },
    { "present",               Wrap_Graphics::Present               },
    { "print",                 Wrap_Graphics::Print                 },
    { "printf",                Wrap_Graphics::Printf                },
    { "push",                  Wrap_Graphics::Push                  },
    { "replaceTransform",      Wrap_Graphics::ReplaceTransform      },
    { "rotate",                Wrap_Graphics::Rotate                },
    { "scale",                 Wrap_Graphics::Scale                 },
    { "setActiveScreen",       Wrap_Graphics::SetActiveScreen       },
    { "setBackgroundColor",    Wrap_Graphics::SetBackgroundColor    },
    { "setColor",              Wrap_Graphics::SetColor              },
    { "shear",                 Wrap_Graphics::Shear                 },
    { "translate",             Wrap_Graphics::Translate             },
    { "getBackgroundColor",    Wrap_Graphics::GetBackgroundColor    },
    { "getColor",              Wrap_Graphics::GetColor              },
    { "getScreens",            Wrap_Graphics::GetScreens            },
    { "getWidth",              Wrap_Graphics::GetWidth              },
    { "getHeight",             Wrap_Graphics::GetHeight             },
    { "getDimensions",         Wrap_Graphics::GetDimensions         },
    { "newFont",               Wrap_Graphics::NewFont               },
    { "newQuad",               Wrap_Graphics::NewQuad               },
    { "newTexture",            Wrap_Graphics::NewTexture            },
    { "setFont",               Wrap_Graphics::SetFont               },
    { "getFont",               Wrap_Graphics::GetFont               },
    { "reset",                 Wrap_Graphics::Reset                 },
    { "transformPoint",        Wrap_Graphics::TransformPoint        }
};

static constexpr lua_CFunction types[] =
{
    Wrap_Font::Register,
    Wrap_Texture::Register,
    Wrap_Quad::Register,
    nullptr
};
// clang-format on

int Wrap_Graphics::Register(lua_State* L)
{
    auto* instance = instance();

    if (instance == nullptr)
        luax::CatchException(L, [&]() { instance = new Graphics<Console::Which>(); });
    else
        instance()->Retain();

    WrappedModule module {};
    module.instance          = instance;
    module.name              = "graphics";
    module.functions         = functions;
    module.extendedFunctions = extensions;
    module.type              = &Module::type;
    module.types             = types;

    return luax::RegisterModule(L, module);
}
