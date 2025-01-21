#include "common/screen.hpp"

#include "modules/graphics/wrap_Graphics.hpp"

#include "modules/filesystem/wrap_Filesystem.hpp"

#include "modules/graphics/wrap_Font.hpp"
#include "modules/graphics/wrap_Quad.hpp"
#include "modules/graphics/wrap_SpriteBatch.hpp"
#include "modules/graphics/wrap_TextBatch.hpp"
#include "modules/graphics/wrap_Texture.hpp"

#include "modules/image/Image.hpp"
#include "modules/image/ImageData.hpp"
#include "modules/image/wrap_CompressedImageData.hpp"
#include "modules/image/wrap_Image.hpp"
#include "modules/image/wrap_ImageData.hpp"

using namespace love;

#define instance() (Module::getInstance<Graphics>(Module::M_GRAPHICS))

static int luax_checkgraphicscreated(lua_State* L)
{
    if (!instance()->isCreated())
        luaL_error(L, "love.graphics cannot function without a window!");

    return 0;
}

int Wrap_Graphics::reset(lua_State*)
{
    instance()->reset();

    return 0;
}

int Wrap_Graphics::clear(lua_State* L)
{
    OptionalColor color(Color(0, 0, 0, 0));
    std::vector<OptionalColor> colors {};

    OptionalInt stencil(0);
    OptionalDouble depth(1.0);

    int argType = lua_type(L, 1);
    int start   = -1;

    if (argType == LUA_TTABLE)
    {
        int max = lua_gettop(L);
        colors.reserve(max);

        for (int index = 0; index < max; index++)
        {
            argType = lua_type(L, index + 1);

            if (argType == LUA_TNUMBER || argType == LUA_TBOOLEAN)
            {
                start = index + 1;
                break;
            }
            else if (argType == LUA_TNIL || argType == LUA_TNONE || luax_objlen(L, index + 1) == 0)
            {
                colors.push_back(OptionalColor());
                continue;
            }

            for (int j = 1; j <= 4; j++)
                lua_rawgeti(L, index + 1, j);

            OptionalColor _color {};
            _color.hasValue = true;

            _color.value.r = luaL_checknumber(L, -4);
            _color.value.g = luaL_checknumber(L, -3);
            _color.value.b = luaL_checknumber(L, -2);
            _color.value.a = luaL_checknumber(L, -1);

            colors.push_back(_color);

            lua_pop(L, 4);
        }
    }
    else if (argType == LUA_TBOOLEAN)
    {
        color.hasValue = luax_toboolean(L, 1);
        start          = 2;
    }
    else if (argType != LUA_TNONE && argType != LUA_TNIL)
    {
        color.hasValue = true;

        color.value.r = luaL_checknumber(L, 1);
        color.value.g = luaL_checknumber(L, 2);
        color.value.b = luaL_checknumber(L, 3);
        color.value.a = luaL_optnumber(L, 4, 1.0);

        start = 5;
    }

    if (start >= 0)
    {
        argType = lua_type(L, start);

        if (argType == LUA_TBOOLEAN)
            stencil.hasValue = luax_toboolean(L, start);
        else if (argType == LUA_TNUMBER)
            stencil.value = luaL_checkinteger(L, start);

        argType = lua_type(L, start + 1);

        if (argType == LUA_TBOOLEAN)
            depth.hasValue = luax_toboolean(L, start + 1);
        else if (argType == LUA_TNUMBER)
            depth.value = luaL_checknumber(L, start + 1);
    }

    if (colors.empty())
        luax_catchexcept(L, [&]() { instance()->clear(color, stencil, depth); });
    else
        luax_catchexcept(L, [&]() { instance()->clear(colors, stencil, depth); });

    return 0;
}

int Wrap_Graphics::present(lua_State* L)
{
    luax_catchexcept(L, [&]() { instance()->present(L); });

    return 0;
}

int Wrap_Graphics::setColor(lua_State* L)
{
    Color color {};

    if (lua_istable(L, 1))
    {
        for (int index = 1; index <= 4; index++)
            lua_rawgeti(L, 1, index);

        color.r = luax_checknumberclamped01(L, -4);
        color.g = luax_checknumberclamped01(L, -3);
        color.b = luax_checknumberclamped01(L, -2);
        color.a = luax_optnumberclamped01(L, -1, 1.0);

        lua_pop(L, 4);
    }
    else
    {
        color.r = luax_checknumberclamped01(L, 1);
        color.g = luax_checknumberclamped01(L, 2);
        color.b = luax_checknumberclamped01(L, 3);
        color.a = luax_optnumberclamped01(L, 4, 1.0);
    }

    instance()->setColor(color);

    return 0;
}

int Wrap_Graphics::getColor(lua_State* L)
{
    Color color = instance()->getColor();

    lua_pushnumber(L, color.r);
    lua_pushnumber(L, color.g);
    lua_pushnumber(L, color.b);
    lua_pushnumber(L, color.a);

    return 4;
}

int Wrap_Graphics::setBackgroundColor(lua_State* L)
{
    Color color {};

    if (lua_istable(L, 1))
    {
        for (int index = 1; index <= 4; index++)
            lua_rawgeti(L, 1, index);

        color.r = luaL_checknumber(L, -4);
        color.g = luaL_checknumber(L, -3);
        color.b = luaL_checknumber(L, -2);
        color.a = luaL_optnumber(L, -1, 1.0);

        lua_pop(L, 4);
    }
    else
    {
        color.r = luaL_checknumber(L, 1);
        color.g = luaL_checknumber(L, 2);
        color.b = luaL_checknumber(L, 3);
        color.a = luaL_optnumber(L, 4, 1.0);
    }

    instance()->setBackgroundColor(color);

    return 0;
}

int Wrap_Graphics::getBackgroundColor(lua_State* L)
{
    auto color = instance()->getBackgroundColor();

    lua_pushnumber(L, color.r);
    lua_pushnumber(L, color.g);
    lua_pushnumber(L, color.b);
    lua_pushnumber(L, color.a);

    return 4;
}

int Wrap_Graphics::setColorMask(lua_State* L)
{
    ColorChannelMask mask {};

    if (lua_isnoneornil(L, 1))
        mask.r = mask.g = mask.b = mask.a = true;
    else if (lua_gettop(L) <= 1)
    {
        mask.r = mask.g = mask.b = mask.a = luax_checkboolean(L, 1);
    }
    else
    {
        mask.r = luax_toboolean(L, 1);
        mask.g = luax_toboolean(L, 2);
        mask.b = luax_toboolean(L, 3);
        mask.a = luax_toboolean(L, 4);
    }

    instance()->setColorMask(mask);

    return 0;
}

int Wrap_Graphics::getColorMask(lua_State* L)
{
    auto mask = instance()->getColorMask();

    lua_pushboolean(L, mask.r);
    lua_pushboolean(L, mask.g);
    lua_pushboolean(L, mask.b);
    lua_pushboolean(L, mask.a);

    return 4;
}

static BlendOperation luax_checkblendop(lua_State* L, int idx)
{
    auto blendOperation = BLENDOP_ADD;
    const char* str     = luaL_checkstring(L, idx);

    if (!love::getConstant(str, blendOperation))
        luax_enumerror(L, "blend operation", BlendOperations, str);

    return blendOperation;
}

static BlendFactor luax_checkblendfactor(lua_State* L, int idx)
{
    BlendFactor factor = BLENDFACTOR_ZERO;
    const char* str    = luaL_checkstring(L, idx);

    if (!love::getConstant(str, factor))
        luax_enumerror(L, "blend factor", BlendFactors, str);

    return factor;
}

static void luax_pushblendop(lua_State* L, BlendOperation operation)
{
    std::string_view name;
    if (!love::getConstant(operation, name))
        luaL_error(L, "unknown blend operation");

    luax_pushstring(L, name);
}

static void luax_pushblendfactor(lua_State* L, BlendFactor factor)
{
    std::string_view name;
    if (!getConstant(factor, name))
        luaL_error(L, "unknown blend factor");

    luax_pushstring(L, name);
}

int Wrap_Graphics::setBlendMode(lua_State* L)
{
    BlendMode mode;
    const char* name = luaL_checkstring(L, 1);

    if (!love::getConstant(name, mode))
        return luax_enumerror(L, "blend mode", BlendModes, name);

    auto alphaMode = BLENDALPHA_MULTIPLY;
    if (!lua_isnoneornil(L, 2))
    {
        const char* alphaName = luaL_checkstring(L, 2);
        if (!love::getConstant(alphaName, alphaMode))
            return luax_enumerror(L, "blend alpha mode", BlendAlphaModes, alphaName);
    }

    luax_catchexcept(L, [&]() { instance()->setBlendMode(mode, alphaMode); });

    return 0;
}

int Wrap_Graphics::getBlendMode(lua_State* L)
{
    std::string_view name;
    std::string_view alphaName;

    BlendAlpha alphaMode;
    const auto mode = instance()->getBlendMode(alphaMode);

    if (!love::getConstant(mode, name))
        luaL_error(L, "Unknown blend mode");

    if (!getConstant(alphaMode, alphaName))
        luaL_error(L, "Unknown blend alpha mode");

    luax_pushstring(L, name);
    luax_pushstring(L, alphaName);

    return 2;
}

int Wrap_Graphics::setBlendState(lua_State* L)
{
    BlendState state {};

    if (!lua_isnoneornil(L, 1))
    {
        state.enable = true;

        if (lua_gettop(L) >= 4)
        {
            state.operationRGB = luax_checkblendop(L, 1);
            state.operationA   = luax_checkblendop(L, 2);
            state.srcFactorRGB = luax_checkblendfactor(L, 3);
            state.srcFactorA   = luax_checkblendfactor(L, 4);
            state.dstFactorRGB = luax_checkblendfactor(L, 5);
            state.dstFactorA   = luax_checkblendfactor(L, 6);
        }
        else
        {
            state.operationRGB = state.operationA = luax_checkblendop(L, 1);
            state.srcFactorRGB = state.srcFactorA = luax_checkblendfactor(L, 2);
            state.dstFactorRGB = state.dstFactorA = luax_checkblendfactor(L, 3);
        }
    }

    luax_catchexcept(L, [&]() { instance()->setBlendState(state); });

    return 0;
}

int Wrap_Graphics::getBlendState(lua_State* L)
{
    const auto& state = instance()->getBlendState();

    if (state.enable)
    {
        luax_pushblendop(L, state.operationRGB);
        luax_pushblendop(L, state.operationA);
        luax_pushblendfactor(L, state.srcFactorRGB);
        luax_pushblendfactor(L, state.srcFactorA);
        luax_pushblendfactor(L, state.dstFactorRGB);
        luax_pushblendfactor(L, state.dstFactorA);
    }
    else
    {
        for (int i = 0; i < 6; i++)
            lua_pushnil(L);
    }

    return 6;
}

int Wrap_Graphics::setLineWidth(lua_State* L)
{
    float width = luaL_checknumber(L, 1);

    luax_catchexcept(L, [&]() { instance()->setLineWidth(width); });

    return 0;
}

int Wrap_Graphics::getLineWidth(lua_State* L)
{
    lua_pushnumber(L, instance()->getLineWidth());

    return 1;
}

int Wrap_Graphics::setLineJoin(lua_State* L)
{
    Graphics::LineJoin join;
    const char* str = luaL_checkstring(L, 1);

    if (!Graphics::getConstant(str, join))
        return luax_enumerror(L, "line join", Graphics::LineJoins, str);

    instance()->setLineJoin(join);

    return 0;
}

int Wrap_Graphics::getLineJoin(lua_State* L)
{
    Graphics::LineJoin join = instance()->getLineJoin();
    std::string_view name;

    if (!Graphics::getConstant(join, name))
        luaL_error(L, "Unknown line join");

    luax_pushstring(L, name);

    return 1;
}

int Wrap_Graphics::setLineStyle(lua_State* L)
{
    Graphics::LineStyle style;
    const char* str = luaL_checkstring(L, 1);

    if (!Graphics::getConstant(str, style))
        return luax_enumerror(L, "line style", Graphics::LineStyles, str);

    instance()->setLineStyle(style);

    return 0;
}

int Wrap_Graphics::getLineStyle(lua_State* L)
{
    Graphics::LineStyle style = instance()->getLineStyle();
    std::string_view name;

    if (!Graphics::getConstant(style, name))
        luaL_error(L, "Unknown line style");

    luax_pushstring(L, name);

    return 1;
}

int Wrap_Graphics::setPointSize(lua_State* L)
{
    float size = luaL_checknumber(L, 1);
    instance()->setPointSize(size);

    return 0;
}

int Wrap_Graphics::getPointSize(lua_State* L)
{
    lua_pushnumber(L, instance()->getPointSize());

    return 1;
}

int Wrap_Graphics::setMeshCullMode(lua_State* L)
{
    CullMode mode;
    const char* str = luaL_checkstring(L, 1);

    if (!love::getConstant(str, mode))
        return luax_enumerror(L, "cull mode", love::CullModes, str);

    instance()->setMeshCullMode(mode);

    return 0;
}

int Wrap_Graphics::getMeshCullMode(lua_State* L)
{
    auto cullMode = instance()->getMeshCullMode();
    std::string_view name;

    if (!love::getConstant(cullMode, name))
        luaL_error(L, "Unknown cull mode");

    luax_pushstring(L, name);

    return 1;
}

int Wrap_Graphics::setFrontFaceWinding(lua_State* L)
{
    Winding winding;
    const char* str = luaL_checkstring(L, 1);

    if (!love::getConstant(str, winding))
        return luax_enumerror(L, "winding", love::WindingModes, str);

    instance()->setFrontFaceWinding(winding);

    return 0;
}

int Wrap_Graphics::getFrontFaceWinding(lua_State* L)
{
    auto winding = instance()->getFrontFaceWinding();
    std::string_view name;

    if (!love::getConstant(winding, name))
        luaL_error(L, "Unknown winding");

    luax_pushstring(L, name);

    return 1;
}

int Wrap_Graphics::isCreated(lua_State* L)
{
    if (!instance()->isCreated())
        return luaL_error(L, "love.graphics cannot function without a window!");

    return 0;
}

int Wrap_Graphics::isActive(lua_State* L)
{
    lua_pushboolean(L, instance()->isActive());

    return 1;
}

int Wrap_Graphics::isGammaCorrect(lua_State* L)
{
    lua_pushboolean(L, love::isGammaCorrect());

    return 1;
}

int Wrap_Graphics::getRendererInfo(lua_State* L)
{
    auto info = instance()->getRendererInfo();

    lua_pushstring(L, info.name.c_str());
    lua_pushstring(L, info.version.c_str());
    lua_pushstring(L, info.vendor.c_str());
    lua_pushstring(L, info.device.c_str());

    return 4;
}

int Wrap_Graphics::getDPIScale(lua_State* L)
{
    lua_pushnumber(L, instance()->getScreenDPIScale());

    return 1;
}

int Wrap_Graphics::setScissor(lua_State* L)
{
    int argc = lua_gettop(L);

    if (argc == 0 || (argc == 4 && lua_isnil(L, 1) && lua_isnil(L, 2) && lua_isnil(L, 3) && lua_isnil(L, 4)))
    {
        instance()->setScissor();
        return 0;
    }

    Rect scissor {};
    scissor.x = luaL_checkinteger(L, 1);
    scissor.y = luaL_checkinteger(L, 2);
    scissor.w = luaL_checkinteger(L, 3);
    scissor.h = luaL_checkinteger(L, 4);

    if (scissor.w < 0 || scissor.h < 0)
        return luaL_error(L, "Can't set scissor with negative width and/or height.");

    instance()->setScissor(scissor);

    return 0;
}

int Wrap_Graphics::intersectScissor(lua_State* L)
{
    Rect scissor {};
    scissor.x = luaL_checkinteger(L, 1);
    scissor.y = luaL_checkinteger(L, 2);
    scissor.w = luaL_checkinteger(L, 3);
    scissor.h = luaL_checkinteger(L, 4);

    if (scissor.w < 0 || scissor.h < 0)
        return luaL_error(L, "Can't set scissor with negative width and/or height.");

    instance()->intersectScissor(scissor);

    return 0;
}

int Wrap_Graphics::getScissor(lua_State* L)
{
    Rect scissor {};
    if (!instance()->getScissor(scissor))
        return 0;

    lua_pushinteger(L, scissor.x);
    lua_pushinteger(L, scissor.y);
    lua_pushinteger(L, scissor.w);
    lua_pushinteger(L, scissor.h);

    return 4;
}

int Wrap_Graphics::getStackDepth(lua_State* L)
{
    lua_pushinteger(L, instance()->getStackDepth());

    return 1;
}

int Wrap_Graphics::push(lua_State* L)
{
    auto stackType   = Graphics::STACK_TRANSFORM;
    const char* name = lua_isnoneornil(L, 1) ? nullptr : luaL_checkstring(L, 1);

    if (name && !Graphics::getConstant(name, stackType))
        return luax_enumerror(L, "graphics stack type", Graphics::StackTypes, name);

    luax_catchexcept(L, [&]() { instance()->push(stackType); });

    /* TODO: check transform type */

    return 0;
}

int Wrap_Graphics::pop(lua_State* L)
{
    luax_catchexcept(L, [&]() { instance()->pop(); });

    return 0;
}

int Wrap_Graphics::rotate(lua_State* L)
{
    float angle = luaL_checknumber(L, 1);
    instance()->rotate(angle);

    return 0;
}

int Wrap_Graphics::scale(lua_State* L)
{
    float sx = luaL_checknumber(L, 1);
    float sy = luaL_optnumber(L, 2, sx);

    instance()->scale(sx, sy);

    return 0;
}

int Wrap_Graphics::translate(lua_State* L)
{
    float dx = luaL_checknumber(L, 1);
    float dy = luaL_checknumber(L, 2);

    instance()->translate(dx, dy);

    return 0;
}

int Wrap_Graphics::shear(lua_State* L)
{
    float kx = luaL_checknumber(L, 1);
    float ky = luaL_checknumber(L, 2);

    instance()->shear(kx, ky);

    return 0;
}

int Wrap_Graphics::origin(lua_State*)
{
    instance()->origin();

    return 0;
}

int Wrap_Graphics::applyTransform(lua_State* L)
{
    // clang-format off
    luax_checkstandardtransform(L, 1, [&](const Matrix4& transform) {
        instance()->applyTransform(transform);
    });
    // clang-format on

    return 0;
}

int Wrap_Graphics::replaceTransform(lua_State* L)
{
    // clang-format off
    luax_checkstandardtransform(L, 1, [&](const Matrix4& transform) {
        instance()->replaceTransform(transform);
    });
    // clang-format on

    return 0;
}

int Wrap_Graphics::transformPoint(lua_State* L)
{
    Vector2 point {};
    point.x = luaL_checknumber(L, 1);
    point.y = luaL_checknumber(L, 2);

    point = instance()->transformPoint(point);

    lua_pushnumber(L, point.x);
    lua_pushnumber(L, point.y);

    return 2;
}

int Wrap_Graphics::inverseTransformPoint(lua_State* L)
{
    Vector2 point {};
    point.x = luaL_checknumber(L, 1);
    point.y = luaL_checknumber(L, 2);

    point = instance()->inverseTransformPoint(point);

    lua_pushnumber(L, point.x);
    lua_pushnumber(L, point.y);

    return 2;
}

static void parseDPIScale(Data*, float*)
{}

// clang-format off
static std::pair<StrongRef<ImageData>, StrongRef<CompressedImageData>>
getImageData(lua_State* L, int index, bool allowCompressed, float* dpiScale)
{
    StrongRef<ImageData> imageData;
    StrongRef<CompressedImageData> compressedImageData;

    if (luax_istype(L, index, ImageData::type))
        imageData.set(luax_checkimagedata(L, index));
    else if (luax_istype(L, index, CompressedImageData::type))
        compressedImageData.set(luax_checkcompressedimagedata(L, index));
    else if (luax_cangetdata(L, index))
    {
        auto* module = Module::getInstance<Image>(Module::M_IMAGE);
        if (module == nullptr)
            luaL_error(L, "Cannot load images without the love.image module.");

        StrongRef<Data> fileData(luax_getdata(L, index), Acquire::NO_RETAIN);

        if (dpiScale != nullptr)
            parseDPIScale(fileData, dpiScale);

        if (allowCompressed && module->isCompressed(fileData))
            luax_catchexcept(L, [&]() { compressedImageData.set(module->newCompressedData(fileData), Acquire::NO_RETAIN); });
        else
            luax_catchexcept(L, [&]() { imageData.set(module->newImageData(fileData), Acquire::NO_RETAIN); });
    }
    else
        imageData.set(luax_checkimagedata(L, index));

    return { imageData, compressedImageData };
}
// clang-format on

static void luax_checktexturesettings(lua_State* L, int index, bool optional, bool checkType,
                                      bool checkDimensions, OptionalBool forceRenderTarget,
                                      Texture::Settings& settings, bool& setDPIScale)
{
    setDPIScale = false;

    if (forceRenderTarget.hasValue)
        settings.renderTarget = forceRenderTarget.value;

    if (optional && lua_isnoneornil(L, index))
        return;

    luax_checktablefields<Texture::SettingType>(L, index, "texture setting name", Texture::getConstant);

    if (!forceRenderTarget.hasValue)
    {
        const char* name      = Texture::getConstant(Texture::SETTING_RENDER_TARGET);
        settings.renderTarget = luax_boolflag(L, index, name, settings.renderTarget);
    }

    lua_getfield(L, index, Texture::getConstant(Texture::SETTING_DEBUGNAME));
    if (!lua_isnoneornil(L, -1))
        settings.debugName = luaL_checkstring(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, index, Texture::getConstant(Texture::SETTING_FORMAT));
    if (!lua_isnoneornil(L, -1))
    {
        const char* formatName = luaL_checkstring(L, -1);
        if (!love::getConstant(formatName, settings.format))
            luax_enumerror(L, "pixel format", formatName);
    }
    lua_pop(L, 1);

    if (checkType)
    {
        lua_getfield(L, index, Texture::getConstant(Texture::SETTING_TYPE));
        if (!lua_isnoneornil(L, -1))
        {
            const char* typeName = luaL_checkstring(L, -1);
            if (!Texture::getConstant(typeName, settings.type))
                luax_enumerror(L, "texture type", Texture::TextureTypes, typeName);
        }
        lua_pop(L, 1);
    }

    // clang-format off
    if (checkDimensions)
    {
        settings.width = luax_checkintflag(L, index, Texture::getConstant(Texture::SETTING_WIDTH));
        settings.height = luax_checkintflag(L, index, Texture::getConstant(Texture::SETTING_HEIGHT));

        if (settings.type == TEXTURE_2D_ARRAY || settings.type == TEXTURE_VOLUME)
            settings.layers = luax_checkintflag(L, index, Texture::getConstant(Texture::SETTING_LAYERS));
    }
    else
    {
        settings.width = luax_intflag(L, index, Texture::getConstant(Texture::SETTING_WIDTH), settings.width);
        settings.height = luax_intflag(L, index, Texture::getConstant(Texture::SETTING_HEIGHT), settings.height);

        if (settings.type == TEXTURE_2D_ARRAY || settings.type == TEXTURE_VOLUME)
            settings.layers = luax_intflag(L, index, Texture::getConstant(Texture::SETTING_LAYERS), settings.layers);
    }
    // clang-format on

    lua_getfield(L, index, Texture::getConstant(Texture::SETTING_MIPMAPS));
    if (!lua_isnoneornil(L, -1))
    {
        if (lua_type(L, -1) == LUA_TBOOLEAN)
            settings.mipmaps = luax_toboolean(L, -1) ? Texture::MIPMAPS_MANUAL : Texture::MIPMAPS_NONE;
        else
        {
            const char* name = luaL_checkstring(L, -1);
            if (!Texture::getConstant(name, settings.mipmaps))
                luax_enumerror(L, "mipmap mode", Texture::MipmapsModes, name);
        }
    }
    lua_pop(L, 1);

    lua_getfield(L, index, Texture::getConstant(Texture::SETTING_MIPMAP_COUNT));
    if (!lua_isnoneornil(L, -1))
        settings.mipmapCount = luaL_checkinteger(L, -1);
    lua_pop(L, 1);

    settings.linear = luax_boolflag(L, index, Texture::getConstant(Texture::SETTING_LINEAR), settings.linear);
    settings.msaa   = luax_intflag(L, index, Texture::getConstant(Texture::SETTING_MSAA), settings.msaa);

    settings.computeWrite =
        luax_boolflag(L, index, Texture::getConstant(Texture::SETTING_COMPUTE_WRITE), settings.computeWrite);

    lua_getfield(L, index, Texture::getConstant(Texture::SETTING_VIEW_FORMATS));
    if (!lua_isnoneornil(L, -1))
    {
        if (lua_type(L, -1) != LUA_TTABLE)
            luaL_argerror(L, index, "expected field 'viewformats' to be a table type");

        for (int index = 1; index <= (int)luax_objlen(L, -1); index++)
        {
            lua_rawgeti(L, -1, index);
            const char* formatName = luaL_checkstring(L, -1);

            PixelFormat format = PIXELFORMAT_UNKNOWN;
            if (!love::getConstant(formatName, format))
                luax_enumerror(L, "pixel format", formatName);

            settings.viewFormats.push_back(format);
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);

    lua_getfield(L, index, Texture::getConstant(Texture::SETTING_READABLE));
    if (!lua_isnoneornil(L, -1))
        settings.readable.set(luax_checkboolean(L, -1));
    lua_pop(L, 1);

    lua_getfield(L, index, Texture::getConstant(Texture::SETTING_DPI_SCALE));
    if (lua_isnumber(L, -1))
    {
        settings.dpiScale = luaL_checknumber(L, -1);
        setDPIScale       = true;
    }
    lua_pop(L, 1);
}

static int pushNewTexture(lua_State* L, TextureBase::Slices* slices, const Texture::Settings& settings)
{
    StrongRef<TextureBase> texture;

    // clang-format off
    luax_catchexcept(L,
        [&]() { texture.set(instance()->newTexture(settings, slices), Acquire::NO_RETAIN); },
        [&](bool) { if (slices) slices->clear(); }
    );
    // clang-format on

    luax_pushtype(L, texture);
    return 1;
}

int Wrap_Graphics::newTexture(lua_State* L)
{
    luax_checkgraphicscreated(L);

    Texture::Slices slices(TEXTURE_2D);
    Texture::Slices* slicesRef = &slices;

    Texture::Settings settings {};
    settings.type    = TEXTURE_2D;
    bool dpiScaleSet = false;

    if (lua_type(L, 1) == LUA_TNUMBER)
    {
        slicesRef = nullptr;

        settings.width  = luaL_checkinteger(L, 1);
        settings.height = luaL_checkinteger(L, 2);

        int start = 3;

        if (lua_type(L, 3) == LUA_TNUMBER)
        {
            settings.layers = luaL_checkinteger(L, 3);
            settings.type   = TEXTURE_2D_ARRAY;
            start           = 4;
        }

        luax_checktexturesettings(L, start, true, true, false, OptionalBool(), settings, dpiScaleSet);
    }
    else
    {
        luax_checktexturesettings(L, 2, true, false, false, OptionalBool(), settings, dpiScaleSet);
        float* autoDpiScale = dpiScaleSet ? nullptr : &settings.dpiScale;

        if (lua_istable(L, 1))
        {
            const int length = luax_objlen(L, 1);

            for (int index = 0; index < length; index++)
            {
                lua_rawgeti(L, 1, index + 1);

                auto data = getImageData(L, -1, true, index == 0 ? autoDpiScale : nullptr);

                if (data.first.get())
                    slices.set(0, index, data.first);
                else
                    slices.set(0, index, data.second->getSlice(0, 0));
            }

            lua_pop(L, length);
        }
        else
        {
            auto data = getImageData(L, 1, true, autoDpiScale);

            if (data.first.get())
                slices.set(0, 0, data.first);
            else
                slices.add(data.second, 0, 0, false, settings.mipmaps != Texture::MIPMAPS_NONE);
        }
    }

    return pushNewTexture(L, slicesRef, settings);
}

int Wrap_Graphics::newQuad(lua_State* L)
{
    luax_checkgraphicscreated(L);

    Quad::Viewport viewport {};
    viewport.x = luaL_checknumber(L, 1);
    viewport.y = luaL_checknumber(L, 2);
    viewport.w = luaL_checknumber(L, 3);
    viewport.h = luaL_checknumber(L, 4);

    double sourceWidth  = 0.0f;
    double sourceHeight = 0.0f;

    int layer = 0;

    if (luax_istype(L, 5, TextureBase::type))
    {
        TextureBase* texture = luax_checktexture(L, 5);
        sourceWidth          = texture->getWidth();
        sourceHeight         = texture->getHeight();
    }
    else if (luax_istype(L, 6, TextureBase::type))
    {
        layer                = (int)luaL_checkinteger(L, 5) - 1;
        TextureBase* texture = luax_checktexture(L, 6);
        sourceWidth          = texture->getWidth();
        sourceHeight         = texture->getHeight();
    }
    else if (!lua_isnoneornil(L, 7))
    {
        layer        = (int)luaL_checkinteger(L, 5) - 1;
        sourceWidth  = luaL_checknumber(L, 6);
        sourceHeight = luaL_checknumber(L, 7);
    }
    else
    {
        sourceWidth  = luaL_checknumber(L, 5);
        sourceHeight = luaL_checknumber(L, 6);
    }

    Quad* quad = instance()->newQuad(viewport, sourceWidth, sourceHeight);
    quad->setLayer(layer);

    luax_pushtype(L, quad);
    quad->release();

    return 1;
}

int Wrap_Graphics::newImage(lua_State* L)
{
    return newTexture(L);
}

int Wrap_Graphics::draw(lua_State* L)
{
    Drawable* drawable   = nullptr;
    TextureBase* texture = nullptr;

    Quad* quad = luax_totype<Quad>(L, 2);
    int start  = 2;

    if (quad != nullptr)
    {
        texture = luax_checktexture(L, 1);
        start   = 3;
    }
    else if (lua_isnil(L, 2) && !lua_isnoneornil(L, 3))
        return luax_typeerror(L, 2, "Quad");
    else
    {
        drawable = luax_checktype<Drawable>(L, 1);
        start    = 2;
    }

    luax_checkstandardtransform(L, start, [&](const Matrix4& transform) {
        if (texture && quad)
            instance()->draw(texture, quad, transform);
        else
            instance()->draw(drawable, transform);
    });

    return 0;
}

int Wrap_Graphics::setFont(lua_State* L)
{
    auto* font = luax_checktype<FontBase>(L, 1);
    instance()->setFont(font);

    return 0;
}

int Wrap_Graphics::getFont(lua_State* L)
{
    FontBase* font = nullptr;
    luax_catchexcept(L, [&]() { font = instance()->getFont(); });

    luax_pushtype(L, font);

    return 1;
}

static BufferDataUsage luax_optdatausage(lua_State* L, int idx, BufferDataUsage def)
{
    const char* usagestr = lua_isnoneornil(L, idx) ? nullptr : luaL_checkstring(L, idx);

    if (usagestr && !getConstant(usagestr, def))
        luax_enumerror(L, "usage hint", BufferUsages, usagestr);

    return def;
}

static PrimitiveType luax_checkmeshdrawmode(lua_State* L, int idx)
{
    const char* modestr = luaL_checkstring(L, idx);

    PrimitiveType mode = PRIMITIVE_TRIANGLES;
    if (!getConstant(modestr, mode))
        luax_enumerror(L, "mesh draw mode", PrimitiveTypes, modestr);

    return mode;
}

static Mesh* newStandardMesh(lua_State* L)
{
    Mesh* mesh = nullptr;

    PrimitiveType drawMode = luax_checkmeshdrawmode(L, 2);
    BufferDataUsage usage  = luax_optdatausage(L, 3, BUFFERDATAUSAGE_DYNAMIC);

    auto format = Mesh::getDefaultVertexFormat();

    if (lua_istable(L, 1))
    {
        size_t vertexCount = luax_objlen(L, 1);

        std::vector<Vertex> vertices {};
        vertices.reserve(vertexCount);

        for (size_t index = 0; index <= vertexCount; index++)
        {
            lua_rawgeti(L, 1, (int)index + 1);

            if (lua_type(L, -1) != LUA_TTABLE)
            {
                luax_typeerror(L, 1, "table of tables");
                return nullptr;
            }

            for (int j = 1; j <= 8; j++)
                lua_rawgeti(L, -j, j);

            Vertex vertex {};
            vertex.x = luaL_checknumber(L, -8);
            vertex.y = luaL_checknumber(L, -7);
            vertex.s = luaL_optnumber(L, -6, 0.0f);
            vertex.t = luaL_optnumber(L, -5, 0.0f);

            vertex.color.r = luax_optnumberclamped01(L, -4, 1.0f);
            vertex.color.g = luax_optnumberclamped01(L, -3, 1.0f);
            vertex.color.b = luax_optnumberclamped01(L, -2, 1.0f);
            vertex.color.a = luax_optnumberclamped01(L, -1, 1.0f);

            lua_pop(L, 9);
            vertices.push_back(vertex);
        }

        // luax_catchexcept(L, [&]() { mesh = instance()->newMesh(format, vertices, drawMode, usage); });
    }
    else
    {
        int count = (int)luaL_checkinteger(L, 1);
        // luax_catchexcept(L, [&]() { mesh = instance()->newMesh(format, count, drawMode, usage); });
    }

    return mesh;
}

int Wrap_Graphics::newMesh(lua_State* L)
{
    luax_checkgraphicscreated(L);

    // int firstArgType = lua_type(L, 1);
    // if (firstArgType != LUA_TTABLE && firstArgType != LUA_TNUMBER)
    //     return luax_typeerror(L, 1, "table or number expected.");

    // Mesh* mesh = nullptr;

    // int secondArgType = lua_type(L, 2);

    // if (firstArgType == LUA_TTABLE &&
    //     (secondArgType == LUA_TTABLE || secondArgType == LUA_TNUMBER || secondArgType == LUA_TUSERDATA))
    // {
    // }
    // else
    //     mesh = newStandardMesh(L);

    // luax_pushtype(L, mesh);
    // mesh->release();

    return 0;
}

int Wrap_Graphics::newTextBatch(lua_State* L)
{
    luax_checkgraphicscreated(L);

    auto* font       = luax_checkfont(L, 1);
    TextBatch* batch = nullptr;

    if (lua_isnoneornil(L, 2))
        luax_catchexcept(L, [&]() { batch = instance()->newTextBatch(font); });
    else
    {
        std::vector<ColoredString> text {};
        luax_checkcoloredstring(L, 2, text);

        luax_catchexcept(L, [&]() { batch = instance()->newTextBatch(font, text); });
    }

    luax_pushtype(L, batch);
    batch->release();

    return 1;
}

int Wrap_Graphics::newSpriteBatch(lua_State* L)
{
    luax_checkgraphicscreated(L);

    TextureBase* texture  = luax_checktexture(L, 1);
    int size              = luaL_optinteger(L, 2, 1000);
    BufferDataUsage usage = BUFFERDATAUSAGE_DYNAMIC;

    if (lua_gettop(L) > 2)
    {
        const char* usageType = luaL_checkstring(L, 3);
        if (!getConstant(usageType, usage))
            return luax_enumerror(L, "usage hint", BufferUsages, usageType);
    }

    SpriteBatch* batch = nullptr;
    luax_catchexcept(L, [&]() { batch = instance()->newSpriteBatch(texture, size, usage); });

    luax_pushtype(L, batch);
    batch->release();

    return 1;
}

int Wrap_Graphics::newFont(lua_State* L)
{
    luax_checkgraphicscreated(L);

    FontBase* font = nullptr;

    if (!luax_istype(L, 1, Rasterizer::type))
    {
        std::vector<int> indices {};

        for (int index = 0; index < lua_gettop(L); index++)
            indices.push_back(index + 1);

        luax_convobj(L, indices, "font", "newRasterizer");
    }

    auto* rasterizer = luax_checktype<Rasterizer>(L, 1);
    luax_catchexcept(L, [&]() { font = instance()->newFont(rasterizer); });

    luax_pushtype(L, font);
    font->release();

    return 1;
}

int Wrap_Graphics::print(lua_State* L)
{
    std::vector<ColoredString> strings {};
    luax_checkcoloredstring(L, 1, strings);

    // clang-format off
    if (luax_istype(L, 2, FontBase::type))
    {
        auto* font = luax_checkfont(L, 2);
        luax_checkstandardtransform(L, 3, [&](const Matrix4& transform) {
            instance()->print(strings, font, transform);
        });
    }
    else
    {
        luax_checkstandardtransform(L, 2, [&](const Matrix4& transform) {
            instance()->print(strings, transform);
        });
    }
    // clang-format on

    return 0;
}

int Wrap_Graphics::printf(lua_State* L)
{
    std::vector<ColoredString> strings {};
    luax_checkcoloredstring(L, 1, strings);

    FontBase* font = nullptr;
    int start      = 2;

    if (luax_istype(L, 2, FontBase::type))
    {
        font = luax_checkfont(L, start);
        start++;
    }

    auto align = FontBase::ALIGN_LEFT;
    Matrix4 matrix;

    int formatIndex = start + 2;

    if (luax_istype(L, start, Transform::type))
    {
        auto* transform = luax_totype<Transform>(L, start);
        matrix          = transform->getMatrix();

        formatIndex = start + 1;
    }
    else
    {
        float x = luaL_checknumber(L, start + 0);
        float y = luaL_checknumber(L, start + 1);

        float angle = luaL_optnumber(L, start + 4, 0.0f);
        float sx    = luaL_optnumber(L, start + 5, 1.0f);
        float sy    = luaL_optnumber(L, start + 6, sx);
        float ox    = luaL_optnumber(L, start + 7, 0.0f);
        float oy    = luaL_optnumber(L, start + 8, 0.0f);
        float kx    = luaL_optnumber(L, start + 9, 0.0f);
        float ky    = luaL_optnumber(L, start + 10, 0.0f);

        matrix = Matrix4(x, y, angle, sx, sy, ox, oy, kx, ky);
    }

    float wrap = luaL_checknumber(L, formatIndex);

    auto* alignment = lua_isnoneornil(L, formatIndex + 1) ? nullptr : luaL_checkstring(L, formatIndex + 1);
    if (alignment != nullptr && !FontBase::getConstant(alignment, align))
        return luax_enumerror(L, "alignment", FontBase::AlignModes, alignment);

    if (font != nullptr)
        luax_catchexcept(L, [&]() { instance()->printf(strings, font, wrap, align, matrix); });
    else
        luax_catchexcept(L, [&]() { instance()->printf(strings, wrap, align, matrix); });

    return 0;
}

int Wrap_Graphics::getStats(lua_State* L)
{
    auto stats = instance()->getStats();

    if (lua_istable(L, 1))
        lua_pushvalue(L, 1);
    else
        lua_createtable(L, 0, 7);

    lua_pushinteger(L, stats.drawCalls);
    lua_setfield(L, -2, "drawcalls");

    lua_pushinteger(L, stats.drawCallsBatched);
    lua_setfield(L, -2, "drawcallsbatched");

    lua_pushinteger(L, stats.shaderSwitches);
    lua_setfield(L, -2, "shaderswitches");

    lua_pushinteger(L, stats.textures);
    lua_setfield(L, -2, "textures");

    lua_pushnumber(L, (lua_Number)stats.textureMemory);
    lua_setfield(L, -2, "texturememory");

    lua_pushnumber(L, (lua_Number)stats.cpuProcessingTime);
    lua_setfield(L, -2, "cpuprocessingtime");

    lua_pushnumber(L, (lua_Number)stats.gpuDrawingTime);
    lua_setfield(L, -2, "gpudrawingtime");

    return 1;
}

int Wrap_Graphics::polygon(lua_State* L)
{
    int argc = lua_gettop(L) - 1;

    Graphics::DrawMode mode;
    const char* name = luaL_checkstring(L, 1);

    if (!Graphics::getConstant(name, mode))
        return luax_enumerror(L, "draw mode", Graphics::DrawModes, name);

    bool isTable = false;

    if (argc == 1 && lua_istable(L, 2))
    {
        argc    = (int)luax_objlen(L, 2);
        isTable = true;
    }

    if (argc % 2 != 0)
        return luaL_error(L, "Number of vertex components must be a multiple of two.");
    else if (argc < 6)
        return luaL_error(L, "Need at least three vertices to draw a polygon.");

    int numVertices = argc / 2;
    auto* coords    = instance()->getScratchBuffer<Vector2>(numVertices + 1);

    if (isTable)
    {
        for (int index = 0; index < numVertices; index++)
        {
            lua_rawgeti(L, 2, (index * 2) + 1);
            lua_rawgeti(L, 2, (index * 2) + 2);

            coords[index].x = luax_checkfloat(L, -2);
            coords[index].y = luax_checkfloat(L, -1);

            lua_pop(L, 2);
        }
    }
    else
    {
        for (int index = 0; index < numVertices; ++index)
        {
            coords[index].x = luax_checkfloat(L, (index * 2) + 2);
            coords[index].y = luax_checkfloat(L, (index * 2) + 3);
        }
    }

    coords[numVertices] = coords[0];

    luax_catchexcept(L, [&]() { instance()->polygon(mode, std::span(coords, numVertices + 1)); });

    return 0;
}

int Wrap_Graphics::rectangle(lua_State* L)
{
    Graphics::DrawMode mode;
    const char* name = luaL_checkstring(L, 1);

    if (!Graphics::getConstant(name, mode))
        return luax_enumerror(L, "draw mode", Graphics::DrawModes, name);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);
    float w = luaL_checknumber(L, 4);
    float h = luaL_checknumber(L, 5);

    if (lua_isnoneornil(L, 6))
    {
        luax_catchexcept(L, [&]() { instance()->rectangle(mode, x, y, w, h); });
        return 0;
    }

    float rx = luaL_optnumber(L, 6, 0.0f);
    float ry = luaL_optnumber(L, 7, rx);

    if (lua_isnoneornil(L, 8))
        luax_catchexcept(L, [&]() { instance()->rectangle(mode, x, y, w, h, rx, ry); });
    else
    {
        int points = luaL_checkinteger(L, 8);
        luax_catchexcept(L, [&]() { instance()->rectangle(mode, x, y, w, h, rx, ry, points); });
    }

    return 0;
}

int Wrap_Graphics::circle(lua_State* L)
{
    Graphics::DrawMode mode;
    const char* name = luaL_checkstring(L, 1);

    if (!Graphics::getConstant(name, mode))
        return luax_enumerror(L, "draw mode", Graphics::DrawModes, name);

    float x      = luaL_checknumber(L, 2);
    float y      = luaL_checknumber(L, 3);
    float radius = luaL_checknumber(L, 4);

    if (lua_isnoneornil(L, 5))
        luax_catchexcept(L, [&]() { instance()->circle(mode, x, y, radius); });
    else
    {
        int points = luaL_checkinteger(L, 5);
        luax_catchexcept(L, [&]() { instance()->circle(mode, x, y, radius, points); });
    }

    return 0;
}

int Wrap_Graphics::ellipse(lua_State* L)
{
    Graphics::DrawMode mode;
    const char* name = luaL_checkstring(L, 1);

    if (!Graphics::getConstant(name, mode))
        return luax_enumerror(L, "draw mode", Graphics::DrawModes, name);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);
    float a = luaL_checknumber(L, 4);
    float b = luaL_optnumber(L, 5, a);

    if (lua_isnoneornil(L, 6))
        luax_catchexcept(L, [&]() { instance()->ellipse(mode, x, y, a, b); });
    else
    {
        int points = luaL_checkinteger(L, 6);
        luax_catchexcept(L, [&]() { instance()->ellipse(mode, x, y, a, b, points); });
    }

    return 0;
}

int Wrap_Graphics::arc(lua_State* L)
{
    Graphics::DrawMode mode;
    const char* name = luaL_checkstring(L, 1);

    if (!Graphics::getConstant(name, mode))
        return luax_enumerror(L, "draw mode", Graphics::DrawModes, name);

    int start = 2;

    Graphics::ArcMode arcMode = Graphics::ARC_PIE;

    if (lua_type(L, 2) == LUA_TSTRING)
    {
        const char* arcName = luaL_checkstring(L, 2);

        if (!Graphics::getConstant(arcName, arcMode))
            return luax_enumerror(L, "arc mode", Graphics::ArcModes, arcName);

        start = 3;
    }

    float x      = luaL_checknumber(L, start + 0);
    float y      = luaL_checknumber(L, start + 1);
    float radius = luaL_checknumber(L, start + 2);
    float angle1 = luaL_checknumber(L, start + 3);
    float angle2 = luaL_checknumber(L, start + 4);

    if (lua_isnoneornil(L, start + 5))
        luax_catchexcept(L, [&]() { instance()->arc(mode, arcMode, x, y, radius, angle1, angle2); });
    else
    {
        int points = luaL_checkinteger(L, start + 5);
        luax_catchexcept(L, [&]() { instance()->arc(mode, arcMode, x, y, radius, angle1, angle2, points); });
    }

    return 0;
}

int Wrap_Graphics::points(lua_State* L)
{
    int argc = lua_gettop(L);

    bool isTable         = false;
    bool isTableOfTables = false;

    if (argc == 1 && lua_istable(L, 1))
    {
        isTable = true;
        argc    = (int)luax_objlen(L, 1);

        lua_rawgeti(L, 1, 1);
        isTableOfTables = lua_istable(L, -1);
        lua_pop(L, 1);
    }

    if (argc % 2 != 0 && !isTableOfTables)
        return luaL_error(L, "Number of vertex components must be a multiple of two.");

    int numPositions = argc / 2;

    if (isTableOfTables)
        numPositions = argc;

    Vector2* positions = nullptr;
    Color* colors      = nullptr;

    if (isTableOfTables)
    {
        size_t size   = (sizeof(Vector2) + sizeof(Color)) * numPositions;
        uint8_t* data = instance()->getScratchBuffer<uint8_t>(size);

        positions = (Vector2*)data;
        colors    = (Color*)(data + sizeof(Vector2) * numPositions);
    }
    else
        positions = instance()->getScratchBuffer<Vector2>(numPositions);

    if (isTable)
    {
        if (isTableOfTables)
        {
            for (int index = 0; index < argc; index++)
            {
                lua_rawgeti(L, 1, index + 1);

                for (int j = 1; j <= 6; j++)
                    lua_rawgeti(L, -j, j);

                positions[index].x = luax_checkfloat(L, -6);
                positions[index].y = luax_checkfloat(L, -5);

                colors[index].r = (float)luax_optnumberclamped01(L, -4, 1.0);
                colors[index].g = (float)luax_optnumberclamped01(L, -3, 1.0);
                colors[index].b = (float)luax_optnumberclamped01(L, -2, 1.0);
                colors[index].a = (float)luax_optnumberclamped01(L, -1, 1.0);

                lua_pop(L, 7);
            }
        }
        else
        {
            for (int index = 0; index < numPositions; index++)
            {
                lua_rawgeti(L, 1, index * 2 + 1);
                lua_rawgeti(L, 1, index * 2 + 2);

                positions[index].x = luax_checkfloat(L, -2);
                positions[index].y = luax_checkfloat(L, -1);

                lua_pop(L, 2);
            }
        }
    }
    else
    {
        for (int index = 0; index < numPositions; index++)
        {
            positions[index].x = luax_checkfloat(L, index * 2 + 1);
            positions[index].y = luax_checkfloat(L, index * 2 + 2);
        }
    }

    luax_catchexcept(L, [&]() { instance()->points(positions, colors, numPositions); });

    return 0;
}

int Wrap_Graphics::line(lua_State* L)
{
    int argc     = lua_gettop(L);
    int arg1Type = lua_type(L, 1);
    bool isTable = false;

    if (argc == 1 && arg1Type == LUA_TTABLE)
    {
        argc    = (int)luax_objlen(L, 1);
        isTable = true;
    }

    if (arg1Type != LUA_TTABLE && arg1Type != LUA_TNUMBER)
        return luax_typeerror(L, 1, "table or number");
    else if (argc % 2 != 0)
        return luaL_error(L, "Number of vertex components must be a multiple of two.");
    else if (argc < 4)
        return luaL_error(L, "Need at least two vertices to draw a line.");

    int numVertices = argc / 2;

    auto* coords = instance()->getScratchBuffer<Vector2>(numVertices);

    if (isTable)
    {
        for (int index = 0; index < numVertices; ++index)
        {
            lua_rawgeti(L, 1, (index * 2) + 1);
            lua_rawgeti(L, 1, (index * 2) + 2);

            coords[index].x = luax_checkfloat(L, -2);
            coords[index].y = luax_checkfloat(L, -1);

            lua_pop(L, 2);
        }
    }
    else
    {
        for (int index = 0; index < numVertices; ++index)
        {
            coords[index].x = luax_checkfloat(L, (index * 2) + 1);
            coords[index].y = luax_checkfloat(L, (index * 2) + 2);
        }
    }

    luax_catchexcept(L, [&]() { instance()->polyline(std::span(coords, numVertices)); });

    return 0;
}

int Wrap_Graphics::getDefaultFilter(lua_State* L)
{
    const auto& state = instance()->getDefaultSamplerState();

    std::string_view minStr {};
    std::string_view magStr {};

    if (!SamplerState::getConstant(state.minFilter, minStr))
        return luaL_error(L, "Unknown minification filter mode.");

    if (!SamplerState::getConstant(state.magFilter, magStr))
        return luaL_error(L, "Unknown magnification filter mode.");

    luax_pushstring(L, minStr);
    luax_pushstring(L, magStr);
    lua_pushnumber(L, state.maxAnisotropy);

    return 3;
}

int Wrap_Graphics::setDefaultFilter(lua_State* L)
{
    auto state = instance()->getDefaultSamplerState();

    const char* minStr = luaL_checkstring(L, 1);
    const char* magStr = luaL_optstring(L, 2, minStr);

    if (!SamplerState::getConstant(minStr, state.minFilter))
        return luax_enumerror(L, "filter mode", SamplerState::FilterModes, minStr);

    if (!SamplerState::getConstant(magStr, state.magFilter))
        return luax_enumerror(L, "filter mode", SamplerState::FilterModes, magStr);

    state.maxAnisotropy = std::clamp((int)luaL_optnumber(L, 3, 1.0), 1, LOVE_UINT8_MAX);
    instance()->setDefaultSamplerState(state);

    return 0;
}

int Wrap_Graphics::getWidth(lua_State* L)
{
    lua_pushinteger(L, instance()->getWidth());

    return 1;
}

int Wrap_Graphics::getHeight(lua_State* L)
{
    lua_pushinteger(L, instance()->getHeight());

    return 1;
}

int Wrap_Graphics::getDimensions(lua_State* L)
{
    lua_pushinteger(L, instance()->getWidth());
    lua_pushinteger(L, instance()->getHeight());

    return 2;
}

int Wrap_Graphics::getPixelWidth(lua_State* L)
{
    lua_pushinteger(L, instance()->getWidth());

    return 1;
}

int Wrap_Graphics::getPixelHeight(lua_State* L)
{
    lua_pushinteger(L, instance()->getHeight());

    return 1;
}

int Wrap_Graphics::getPixelDimensions(lua_State* L)
{
    lua_pushinteger(L, instance()->getWidth());
    lua_pushinteger(L, instance()->getHeight());

    return 2;
}

// Homebrew Stuff™

int Wrap_Graphics::getScreens(lua_State* L)
{
    auto screens = love::getScreenInfo();
    lua_createtable(L, screens.size(), 0);

    for (size_t i = 0; i < screens.size(); i++)
    {
        luax_pushstring(L, screens[i].name);
        lua_rawseti(L, -2, i + 1);
    }

    return 1;
}

int Wrap_Graphics::getActiveScreen(lua_State* L)
{
    auto& info = love::getScreenInfo(love::currentScreen);
    luax_pushstring(L, info.name);

    return 1;
}

int Wrap_Graphics::setActiveScreen(lua_State* L)
{
    std::string name = luax_checkstring(L, 1);
    auto value       = love::getScreenId(name);

    if (value == INVALID_SCREEN)
        return luaL_error(L, "Invalid screen '%s'", name.c_str());

    love::currentScreen = value;
    instance()->setActiveScreen();

    return 0;
}

#if !defined(__WIIU__)
int Wrap_Graphics::copyCurrentScanBuffer(lua_State* L)
{
    return 0;
}
#endif

// clang-format off
#if defined(__3DS__)
int Wrap_Graphics::is3D(lua_State* L)
{
    lua_pushboolean(L, instance()->is3D());

    return 1;
}

int Wrap_Graphics::set3D(lua_State* L)
{
    bool enable = luax_toboolean(L, 1);
    instance()->set3D(enable);

    return 0;
}

int Wrap_Graphics::isWide(lua_State* L)
{
    lua_pushboolean(L, instance()->isWide());

    return 1;
}

int Wrap_Graphics::setWide(lua_State* L)
{
    bool enable = luax_toboolean(L, 1);
    instance()->setWide(enable);

    return 0;
}

int Wrap_Graphics::getDepth(lua_State* L)
{
    lua_pushnumber(L, instance()->getDepth());

    return 1;
}

static constexpr std::array<luaL_Reg, 5> platformFunctions =
{{
    { "is3D",     Wrap_Graphics::is3D     },
    { "set3D",    Wrap_Graphics::set3D    },
    { "isWide",   Wrap_Graphics::isWide   },
    { "setWide",  Wrap_Graphics::setWide  },
    { "getDepth", Wrap_Graphics::getDepth }
}};
#elif defined(__WIIU__)
int Wrap_Graphics::copyCurrentScanBuffer(lua_State* L)
{
    instance()->copyCurrentScanBuffer();

    return 0;
}
#endif

#if !defined(__3DS__)
static constexpr std::span<const luaL_Reg> platformFunctions = {};
#endif

static constexpr luaL_Reg functions[] =
{
    { "reset",                  Wrap_Graphics::reset                 },
    { "clear",                  Wrap_Graphics::clear                 },
    { "present",                Wrap_Graphics::present               },
    { "setColor",               Wrap_Graphics::setColor              },
    { "getColor",               Wrap_Graphics::getColor              },
    { "setBackgroundColor",     Wrap_Graphics::setBackgroundColor    },
    { "getBackgroundColor",     Wrap_Graphics::getBackgroundColor    },
    { "setColorMask",           Wrap_Graphics::setColorMask          },
    { "getColorMask",           Wrap_Graphics::getColorMask          },
    { "setBlendMode",           Wrap_Graphics::setBlendMode          },
    { "getBlendMode",           Wrap_Graphics::getBlendMode          },
    { "setBlendState",          Wrap_Graphics::setBlendState         },
    { "getBlendState",          Wrap_Graphics::getBlendState         },
    { "setLineWidth",           Wrap_Graphics::setLineWidth          },
    { "getLineWidth",           Wrap_Graphics::getLineWidth          },
    { "setLineJoin",            Wrap_Graphics::setLineJoin           },
    { "getLineJoin",            Wrap_Graphics::getLineJoin           },
    { "setLineStyle",           Wrap_Graphics::setLineStyle          },
    { "getLineStyle",           Wrap_Graphics::getLineStyle          },
    { "setPointSize",           Wrap_Graphics::setPointSize          },
    { "getPointSize",           Wrap_Graphics::getPointSize          },
    { "setMeshCullMode",        Wrap_Graphics::setMeshCullMode       },
    { "getMeshCullMode",        Wrap_Graphics::getMeshCullMode       },
    { "setFrontFaceWinding",    Wrap_Graphics::setFrontFaceWinding   },
    { "getFrontFaceWinding",    Wrap_Graphics::getFrontFaceWinding   },
    { "isCreated",              Wrap_Graphics::isCreated             },
    { "isActive",               Wrap_Graphics::isActive              },
    { "isGammaCorrect",         Wrap_Graphics::isGammaCorrect        },
    { "getRendererInfo",        Wrap_Graphics::getRendererInfo       },
    { "getDPIScale",            Wrap_Graphics::getDPIScale           },
    { "setScissor",             Wrap_Graphics::setScissor            },
    { "intersectScissor",       Wrap_Graphics::intersectScissor      },
    { "getScissor",             Wrap_Graphics::getScissor            },
    { "getStackDepth",          Wrap_Graphics::getStackDepth         },
    { "push",                   Wrap_Graphics::push                  },
    { "pop",                    Wrap_Graphics::pop                   },
    { "rotate",                 Wrap_Graphics::rotate                },
    { "scale",                  Wrap_Graphics::scale                 },
    { "translate",              Wrap_Graphics::translate             },
    { "shear",                  Wrap_Graphics::shear                 },
    { "origin",                 Wrap_Graphics::origin                },
    { "applyTransform",         Wrap_Graphics::applyTransform        },
    { "replaceTransform",       Wrap_Graphics::replaceTransform      },
    { "transformPoint",         Wrap_Graphics::transformPoint        },
    { "inverseTransformPoint",  Wrap_Graphics::inverseTransformPoint },
    { "getStats",               Wrap_Graphics::getStats              },

    { "getWidth",               Wrap_Graphics::getWidth              },
    { "getHeight",              Wrap_Graphics::getHeight             },
    { "getDimensions",          Wrap_Graphics::getDimensions         },
    { "getPixelWidth",          Wrap_Graphics::getPixelWidth         },
    { "getPixelHeight",         Wrap_Graphics::getPixelHeight        },
    { "getPixelDimensions",     Wrap_Graphics::getPixelDimensions    },

    { "setDefaultFilter",       Wrap_Graphics::setDefaultFilter      },
    { "getDefaultFilter",       Wrap_Graphics::getDefaultFilter      },

    { "draw",                   Wrap_Graphics::draw                  },

    { "polygon",                Wrap_Graphics::polygon               },
    { "rectangle",              Wrap_Graphics::rectangle             },
    { "circle",                 Wrap_Graphics::circle                },
    { "ellipse",                Wrap_Graphics::ellipse               },
    { "arc",                    Wrap_Graphics::arc                   },
    { "points",                 Wrap_Graphics::points                },
    { "line",                   Wrap_Graphics::line                  },

    { "newTexture",             Wrap_Graphics::newTexture            },
    { "newQuad",                Wrap_Graphics::newQuad               },
    { "newImage",               Wrap_Graphics::newImage              },

    // { "newMesh",                Wrap_Graphics::newMesh               },

    { "newTextBatch",           Wrap_Graphics::newTextBatch          },
    { "newSpriteBatch",         Wrap_Graphics::newSpriteBatch        },

    { "newFont",                Wrap_Graphics::newFont               },
    { "setFont",                Wrap_Graphics::setFont               },
    { "getFont",                Wrap_Graphics::getFont               },
    { "print",                  Wrap_Graphics::print                 },
    { "printf",                 Wrap_Graphics::printf                },

    { "getScreens",            Wrap_Graphics::getScreens             },
    { "getActiveScreen",       Wrap_Graphics::getActiveScreen        },
    { "setActiveScreen",       Wrap_Graphics::setActiveScreen        },
    { "copyCurrentScanBuffer", Wrap_Graphics::copyCurrentScanBuffer  }
};

static int open_drawable(lua_State* L)
{
    return luax_register_type(L, &Drawable::type);
}

static constexpr lua_CFunction types[] =
{
    open_drawable,
    love::open_texture,
    love::open_quad,
    love::open_font,
    love::open_textbatch,
    love::open_spritebatch
};
// clang-format on

int Wrap_Graphics::open(lua_State* L)
{
    auto* instance = instance();
    if (instance == nullptr)
        luax_catchexcept(L, [&]() { instance = new Graphics(); });
    else
        instance->retain();

    WrappedModule module {};
    module.instance          = instance;
    module.name              = "graphics";
    module.type              = &Module::type;
    module.functions         = functions;
    module.platformFunctions = platformFunctions;
    module.types             = types;

    return luax_register_module(L, module);
}
