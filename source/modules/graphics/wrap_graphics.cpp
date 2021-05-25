#include "modules/graphics/wrap_graphics.h"

#include <filesystem>

using namespace love;

#define instance() (Module::GetInstance<Graphics>(Module::M_GRAPHICS))

int Wrap_Graphics::GetScreens(lua_State* L)
{
    auto screens = instance()->GetScreens();

    lua_createtable(L, screens.size(), 0);

    for (size_t index = 0; index < screens.size(); index++)
    {
        lua_pushstring(L, screens[index]);
        lua_rawseti(L, -2, (index + 1));
    }

    return 1;
}

int Wrap_Graphics::GetActiveScreen(lua_State* L)
{

    Graphics::Screen screen = static_cast<Graphics::Screen>(0);
    const char* name        = nullptr;

    Luax::CatchException(L, [&]() { screen = instance()->GetActiveScreen(); });

    if (!Graphics::GetConstant(screen, name))
        return Luax::EnumError(L, "screen", Graphics::GetConstants(screen), name);

    lua_pushstring(L, name);

    return 1;
}

int Wrap_Graphics::SetActiveScreen(lua_State* L)
{
    Graphics::Screen screen = static_cast<Graphics::Screen>(0);
    const char* name        = luaL_checkstring(L, 1);

#if defined(_3DS)
    auto instance = (love::citro2d::Graphics*)instance();
    if (instance->Get3D())
    {
        if (!Graphics::GetConstant(name, screen))
            return Luax::EnumError(L, "screen", Graphics::GetConstants(screen), name);
    }
    else
    {
        if (!love::citro2d::Graphics::GetConstant(name, screen))
            return Luax::EnumError(L, "screen", love::citro2d::Graphics::GetConstants(screen),
                                   name);
    }
#elif defined(__SWITCH__)
    if (!Graphics::GetConstant(name, screen))
        return Luax::EnumError(L, "screen", Graphics::GetConstants(screen), name);
#endif

    Luax::CatchException(L, [&]() { instance()->SetActiveScreen(screen); });

    return 0;
}

int Wrap_Graphics::GetDimensions(lua_State* L)
{
    Graphics::Screen screen = static_cast<Graphics::Screen>(0);

    const char* sname = lua_isnoneornil(L, 1) ? nullptr : luaL_checkstring(L, 1);

    if (sname == nullptr)
        screen = instance()->GetActiveScreen();
    else
    {
        if (!Graphics::GetConstant(sname, screen))
            return Luax::EnumError(L, "screen", Graphics::GetConstants(screen), sname);
    }

    lua_pushnumber(L, instance()->GetWidth(screen));
    lua_pushnumber(L, instance()->GetHeight());

    return 2;
}

int Wrap_Graphics::GetWidth(lua_State* L)
{
    Graphics::Screen screen = static_cast<Graphics::Screen>(0);

    const char* sname = lua_isnoneornil(L, 1) ? nullptr : luaL_checkstring(L, 1);

    if (sname == nullptr)
        screen = instance()->GetActiveScreen();
    else
    {
        if (!Graphics::GetConstant(sname, screen))
            return Luax::EnumError(L, "screen", Graphics::GetConstants(screen), sname);
    }

    lua_pushnumber(L, instance()->GetWidth(screen));

    return 1;
}

int Wrap_Graphics::GetHeight(lua_State* L)
{
    lua_pushnumber(L, instance()->GetHeight());

    return 1;
}

int Wrap_Graphics::Arc(lua_State* L)
{
    Graphics::DrawMode drawMode;
    const char* mode = luaL_checkstring(L, 1);
    if (!Graphics::GetConstant(mode, drawMode))
        return Luax::EnumError(L, "draw mode", Graphics::GetConstants(drawMode), mode);

    int startidx = 2;

    Graphics::ArcMode arcmode = Graphics::ARC_PIE;

    if (lua_type(L, 2) == LUA_TSTRING)
    {
        const char* arcstr = luaL_checkstring(L, 2);
        if (!Graphics::GetConstant(arcstr, arcmode))
            return Luax::EnumError(L, "arc mode", Graphics::GetConstants(arcmode), arcstr);

        startidx = 3;
    }

    float x      = luaL_checknumber(L, startidx + 0);
    float y      = luaL_checknumber(L, startidx + 1);
    float radius = luaL_checknumber(L, startidx + 2);
    float angle1 = luaL_checknumber(L, startidx + 3);
    float angle2 = luaL_checknumber(L, startidx + 4);

    if (lua_isnoneornil(L, startidx + 5))
    {
        Luax::CatchException(
            L, [&]() { instance()->Arc(drawMode, arcmode, x, y, radius, angle1, angle2); });
    }
    else
    {
        int points = (int)luaL_checkinteger(L, startidx + 5);
        Luax::CatchException(
            L, [&]() { instance()->Arc(drawMode, arcmode, x, y, radius, angle1, angle2, points); });
    }

    return 0;
}

int Wrap_Graphics::Ellipse(lua_State* L)
{
    const char* mode            = luaL_checkstring(L, 1);
    Graphics::DrawMode drawMode = Graphics::DRAW_FILL;

    if (!Graphics::GetConstant(mode, drawMode))
        return luaL_error(L, "Invalid draw mode %s", mode);

    float x = (float)luaL_checknumber(L, 2);
    float y = (float)luaL_checknumber(L, 3);
    float a = (float)luaL_checknumber(L, 4);
    float b = (float)luaL_optnumber(L, 5, a);

    if (lua_isnoneornil(L, 6))
        Luax::CatchException(L, [&]() { instance()->Ellipse(drawMode, x, y, a, b); });
    else
    {
        int points = (int)luaL_checkinteger(L, 6);
        Luax::CatchException(L, [&]() { instance()->Ellipse(drawMode, x, y, a, b, points); });
    }

    return 0;
}

int Wrap_Graphics::Rectangle(lua_State* L)
{
    const char* mode            = luaL_checkstring(L, 1);
    Graphics::DrawMode drawMode = Graphics::DRAW_FILL;

    if (!Graphics::GetConstant(mode, drawMode))
        return luaL_error(L, "Invalid draw mode %s", mode);

    float x = luaL_optnumber(L, 2, 0);
    float y = luaL_optnumber(L, 3, 0);

    float width  = luaL_checknumber(L, 4);
    float height = luaL_checknumber(L, 5);

    if (lua_isnoneornil(L, 6))
    {
        instance()->Rectangle(drawMode, x, y, width, height);
        return 0;
    }

    float rx = (float)luaL_optnumber(L, 6, 0.0);
    float ry = (float)luaL_optnumber(L, 7, rx);

    if (lua_isnoneornil(L, 8))
        Luax::CatchException(
            L, [&]() { instance()->Rectangle(drawMode, x, y, width, height, rx, ry); });
    else
    {
        int points = (int)luaL_checkinteger(L, 8);
        Luax::CatchException(
            L, [&]() { instance()->Rectangle(drawMode, x, y, width, height, rx, ry, points); });
    }

    return 0;
}

int Wrap_Graphics::Circle(lua_State* L)
{
    const char* mode            = luaL_checkstring(L, 1);
    Graphics::DrawMode drawMode = Graphics::DRAW_FILL;

    if (!Graphics::GetConstant(mode, drawMode))
        return luaL_error(L, "Invalid draw mode %s", mode);

    float x      = luaL_checknumber(L, 2);
    float y      = luaL_checknumber(L, 3);
    float radius = luaL_checknumber(L, 4);

    if (lua_isnoneornil(L, 5))
        Luax::CatchException(L, [&]() { instance()->Circle(drawMode, x, y, radius); });
    else
    {
        int points = (int)luaL_checkinteger(L, 5);
        Luax::CatchException(L, [&]() { instance()->Circle(drawMode, x, y, radius, points); });
    }

    return 0;
}

int Wrap_Graphics::GetPointSize(lua_State* L)
{
    float size = instance()->GetPointSize();
    lua_pushnumber(L, size);

    return 1;
}

int Wrap_Graphics::SetPointSize(lua_State* L)
{
    float size = luaL_checknumber(L, 1);
    instance()->SetPointSize(size);

    return 0;
}

int Wrap_Graphics::Points(lua_State* L)
{
    // love.graphics.points has 3 variants:
    // - points(x1, y1, x2, y2, ...)
    // - points({x1, y1, x2, y2, ...})
    // - points({{x1, y1 [, r, g, b, a]}, {x2, y2 [, r, g, b, a]}, ...})

    int args             = lua_gettop(L);
    bool isTable         = false;
    bool isTableOfTables = false;

    if (args == 1 && lua_istable(L, 1))
    {
        isTable = true;
        args    = (int)lua_objlen(L, 1);

        lua_rawgeti(L, 1, 1);
        isTableOfTables = lua_istable(L, -1);
        lua_pop(L, 1);
    }

    if (args % 2 != 0 && !isTableOfTables)
        return luaL_error(L, "Number of vertex components must be a multiple of two");

    int points = args / 2;
    if (isTableOfTables)
        points = args;

    Vector2* positions = nullptr;
    Colorf* colors     = nullptr;

    if (isTableOfTables)
    {
        positions = new Vector2[points * (sizeof(Colorf) + sizeof(Vector2))];
        colors    = new Colorf[sizeof(Vector2) * points];
    }
    else
    {
        positions = new Vector2[points];
        colors    = new Colorf[1] { instance()->GetColor() };
    }

    if (isTable)
    {
        if (isTableOfTables)
        {

            // points({{x1, y1 [, r, g, b, a]}, {x2, y2 [, r, g, b, a]}, ...})
            for (int i = 0; i < args; i++)
            {
                lua_rawgeti(L, 1, i + 1);
                for (int j = 1; j <= 6; j++)
                    lua_rawgeti(L, -j, j);

                positions[i].x = luaL_checknumber(L, -6);
                positions[i].y = luaL_checknumber(L, -5);

                colors[i].r = Luax::OptNumberClamped01(L, -4, 1.0);
                colors[i].g = Luax::OptNumberClamped01(L, -3, 1.0);
                colors[i].b = Luax::OptNumberClamped01(L, -2, 1.0);
                colors[i].a = Luax::OptNumberClamped01(L, -1, 1.0);

                lua_pop(L, 7);
            }
        }
        else
        {
            // points({x1, y1, x2, y2, ...})
            for (int i = 0; i < points; i++)
            {
                lua_rawgeti(L, 1, i * 2 + 1);
                lua_rawgeti(L, 1, i * 2 + 2);

                positions[i].x = luaL_checknumber(L, -2);
                positions[i].y = luaL_checknumber(L, -1);

                lua_pop(L, 2);
            }
        }
    }
    else
    {
        for (int i = 0; i < points; i++)
        {
            positions[i].x = luaL_checknumber(L, i * 2 + 1);
            positions[i].y = luaL_checknumber(L, i * 2 + 2);
        }
    }

    Luax::CatchException(L, [&]() { instance()->Points(positions, points, colors, points); });

    return 0;
}

int Wrap_Graphics::Line(lua_State* L)
{
    int args    = lua_gettop(L);
    int argType = lua_type(L, 1);

    bool is_table = false;

    if (args == 1 && argType == LUA_TTABLE)
    {
        args     = (int)lua_objlen(L, 1);
        is_table = true;
    }

    if (argType != LUA_TTABLE && argType != LUA_TNUMBER)
        return Luax::TypeErrror(L, 1, "table or number");
    else if (args % 2 != 0)
        return luaL_error(L, "Number of vertex components must be a multiple of two.");
    else if (args < 4)
        return luaL_error(L, "Need at least two vertices to draw a line.");

    int numvertices = args / 2;

    Vector2* coords = new Vector2[numvertices];

    if (is_table)
    {
        for (int i = 0; i < numvertices; ++i)
        {
            lua_rawgeti(L, 1, (i * 2) + 1);
            lua_rawgeti(L, 1, (i * 2) + 2);
            coords[i].x = luaL_checknumber(L, -2);
            coords[i].y = luaL_checknumber(L, -1);

            lua_pop(L, 2);
        }
    }
    else
    {
        for (int i = 0; i < numvertices; ++i)
        {
            coords[i].x = luaL_checknumber(L, (i * 2) + 1);
            coords[i].y = luaL_checknumber(L, (i * 2) + 2);
        }
    }

    Luax::CatchException(L, [&]() { instance()->Line(coords, numvertices); });

    delete[] coords;

    return 0;
}

int Wrap_Graphics::Polygon(lua_State* L)
{
    int argc                    = lua_gettop(L) - 1;
    const char* mode            = luaL_checkstring(L, 1);
    Graphics::DrawMode drawMode = Graphics::DRAW_FILL;

    if (!Graphics::GetConstant(mode, drawMode))
        return luaL_error(L, "Invalid draw mode %s", mode);

    bool isTable = false;
    if (argc == 1 && lua_istable(L, 2))
    {
        argc    = (int)lua_objlen(L, 2);
        isTable = true;
    }

    if (argc % 2 != 0)
        return luaL_error(L, "Number of vertex components must be a multiple of two");
    else if (argc < 6)
        return luaL_error(L, "Need at least three vertices to draw a polygon");

    const int numvertices = argc / 2;

    Vector2 points[numvertices + 1];

    if (isTable)
    {
        float x = 0;
        float y = 0;

        for (int i = 0; i < numvertices; ++i)
        {
            lua_rawgeti(L, 2, (i * 2) + 1);
            lua_rawgeti(L, 2, (i * 2) + 2);

            x = luaL_checkinteger(L, -2);
            y = luaL_checkinteger(L, -1);

            points[i].x = x;
            points[i].y = y;

            lua_pop(L, 2);
        }
    }
    else
    {
        float x = 0;
        float y = 0;

        for (int i = 0; i < numvertices; ++i)
        {
            x = luaL_checkinteger(L, (i * 2) + 2);
            y = luaL_checkinteger(L, (i * 2) + 3);

            points[i].x = x;
            points[i].y = y;
        }
    }

    // make a closed loop
    points[numvertices] = points[0];

    Luax::CatchException(L, [&]() { instance()->Polygon(drawMode, points, numvertices + 1); });

    return 0;
}

int Wrap_Graphics::SetLineWidth(lua_State* L)
{
    float width = luaL_checknumber(L, 1);

    instance()->SetLineWidth(width);

    return 0;
}

int Wrap_Graphics::GetLineWidth(lua_State* L)
{
    lua_pushnumber(L, instance()->GetLineWidth());

    return 1;
}

int Wrap_Graphics::Push(lua_State* L)
{
    Graphics::StackType stype = Graphics::STACK_TRANSFORM;
    const char* sname         = lua_isnoneornil(L, 1) ? nullptr : luaL_checkstring(L, 1);

    if (sname && !Graphics::GetConstant(sname, stype))
        return Luax::EnumError(L, "graphics stack type", Graphics::GetConstants(stype), sname);

    Luax::CatchException(L, [&]() { instance()->Push(stype); });

    if (Luax::IsType(L, 2, Transform::type))
    {
        Transform* t = Luax::ToType<Transform>(L, 2);
        Luax::CatchException(L, [&]() { instance()->ApplyTransform(t); });
    }

    return 0;
}

int Wrap_Graphics::ApplyTransform(lua_State* L)
{
    Transform* t = Wrap_Transform::CheckTransform(L, 1);

    Luax::CatchException(L, [&]() { instance()->ApplyTransform(t); });

    return 0;
}

int Wrap_Graphics::ReplaceTransform(lua_State* L)
{
    Transform* t = Wrap_Transform::CheckTransform(L, 1);

    Luax::CatchException(L, [&]() { instance()->ReplaceTransform(t); });

    return 0;
}

int Wrap_Graphics::Origin(lua_State* L)
{
    instance()->Origin();

    return 0;
}

int Wrap_Graphics::Translate(lua_State* L)
{
    float x = luaL_checknumber(L, 1);
    float y = luaL_checknumber(L, 2);

    instance()->Translate(x, y);

    return 0;
}

int Wrap_Graphics::Rotate(lua_State* L)
{
    float angle = luaL_checknumber(L, 1);

    instance()->Rotate(angle);

    return 0;
}

int Wrap_Graphics::Scale(lua_State* L)
{
    float x = luaL_checknumber(L, 1);
    float y = luaL_optnumber(L, 2, x);

    instance()->Scale(x, y);

    return 0;
}

int Wrap_Graphics::Shear(lua_State* L)
{
    float x = luaL_checknumber(L, 1);
    float y = luaL_checknumber(L, 2);

    instance()->Shear(x, y);

    return 0;
}

int Wrap_Graphics::Pop(lua_State* L)
{
    Luax::CatchException(L, [&]() { instance()->Pop(); });

    return 0;
}

int Wrap_Graphics::Clear(lua_State* L)
{
    std::optional<Colorf> color(Colorf(0, 0, 0, 0));

    std::optional<int> stencil(0);
    std::optional<double> depth(1.0);

    int argtype = lua_type(L, 1);
    int start   = -1;

    Graphics::CURRENT_DEPTH = 0.0f;

    if (argtype != LUA_TNONE && argtype != LUA_TNIL)
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
        if (argtype == LUA_TNUMBER)
            stencil.value() = (int)luaL_checkinteger(L, start);

        argtype = lua_type(L, start + 1);
        if (argtype == LUA_TNUMBER)
            depth.value() = luaL_checknumber(L, start + 1);
    }

    Luax::CatchException(L, [&]() { instance()->Clear(color, stencil, depth); });

    return 0;
}

int Wrap_Graphics::Present(lua_State* L)
{
    Luax::CatchException(L, [&]() { instance()->Present(); });

    return 0;
}

int Wrap_Graphics::SetScissor(lua_State* L)
{
    int argCount = lua_gettop(L);

    if (argCount == 0 || (argCount == 4 && Luax::ArgcIsNil<4>(L)))
    {
        instance()->SetScissor();

        return 0;
    }

    Rect rect;

    rect.x = luaL_checkinteger(L, 1);
    rect.y = luaL_checkinteger(L, 2);
    rect.w = luaL_checkinteger(L, 3);
    rect.h = luaL_checkinteger(L, 4);

    if (rect.w < 0 || rect.h < 0)
        return luaL_error(L, "Can't set scissor with negative width and/or height.");

    instance()->SetScissor(rect);

    return 0;
}

int Wrap_Graphics::IntersectScissor(lua_State* L)
{
    Rect rect;

    rect.x = luaL_checkinteger(L, 1);
    rect.y = luaL_checkinteger(L, 2);
    rect.w = luaL_checkinteger(L, 3);
    rect.h = luaL_checkinteger(L, 4);

    if (rect.w < 0 || rect.h < 0)
        return luaL_error(L, "Can't set scissor with negative width and/or height.");

    instance()->IntersectScissor(rect);

    return 0;
}

int Wrap_Graphics::GetScissor(lua_State* L)
{
    Rect scissor;

    if (!instance()->GetScissor(scissor))
        return 0;

    lua_pushnumber(L, scissor.x);
    lua_pushnumber(L, scissor.y);
    lua_pushnumber(L, scissor.w);
    lua_pushnumber(L, scissor.h);

    return 4;
}

int Wrap_Graphics::SetDefaultFilter(lua_State* L)
{
    Texture::Filter filter;

    const char* min = luaL_checkstring(L, 1);
    const char* mag = luaL_optstring(L, 2, min);

    if (!Texture::GetConstant(min, filter.min))
        return Luax::EnumError(L, "filter mode", Texture::GetConstants(filter.min), min);

    if (!Texture::GetConstant(mag, filter.mag))
        return Luax::EnumError(L, "filter mode", Texture::GetConstants(filter.mag), mag);

    instance()->SetDefaultFilter(filter);

    return 0;
}

int Wrap_Graphics::GetDefaultFilter(lua_State* L)
{
    const Texture::Filter& filter = instance()->GetDefaultFilter();

    const char* min;
    const char* mag;

    if (!Texture::GetConstant(filter.min, min))
        return luaL_error(L, "Unknown minification filter mode.");

    if (!Texture::GetConstant(filter.mag, mag))
        return luaL_error(L, "Unknown magnification filter mode.");

    lua_pushstring(L, min);
    lua_pushstring(L, mag);

    return 2;
}

int Wrap_Graphics::NewImage(lua_State* L)
{
    StrongReference<Image> image;

    if (Wrap_Filesystem::CanGetData(L, 1))
    {
        Luax::CatchException(L, [&]() {
            Data* data = Wrap_Filesystem::GetData(L, 1);

            image.Set(instance()->NewImage(data), Acquire::NORETAIN);
        });
    }

    Luax::PushType(L, image);

    return 1;
}

int Wrap_Graphics::NewText(lua_State* L)
{
    Font* font = Wrap_Font::CheckFont(L, 1);
    Text* text = nullptr;

    if (lua_isnoneornil(L, 2))
        Luax::CatchException(L, [&]() { text = instance()->NewText(font); });
    else
    {
        std::vector<Font::ColoredString> strings;
        Wrap_Font::CheckColoredString(L, 2, strings);

        Luax::CatchException(L, [&]() { text = instance()->NewText(font, strings); });
    }

    Luax::PushType(L, text);
    text->Release();

    return 1;
}

int Wrap_Graphics::NewCanvas(lua_State* L)
{
    Canvas::Settings settings;

    int width  = instance()->GetWidth(instance()->GetActiveScreen());
    int height = instance()->GetHeight();

    settings.width  = luaL_optinteger(L, 1, width);
    settings.height = luaL_optinteger(L, 2, height);

    Canvas* canvas = nullptr;

    Luax::CatchException(L, [&]() { canvas = instance()->NewCanvas(settings); });

    Luax::PushType(L, canvas);
    canvas->Release();

    return 1;
}

int Wrap_Graphics::TransformPoint(lua_State* L)
{
    Vector2 point;
    point.x = (float)luaL_checknumber(L, 1);
    point.y = (float)luaL_checknumber(L, 2);

    point = instance()->TransformPoint(point);

    lua_pushnumber(L, point.x);
    lua_pushnumber(L, point.y);

    return 2;
}

int Wrap_Graphics::InverseTransformPoint(lua_State* L)
{
    Vector2 point;
    point.x = (float)luaL_checknumber(L, 1);
    point.y = (float)luaL_checknumber(L, 2);

    point = instance()->InverseTransformPoint(point);

    lua_pushnumber(L, point.x);
    lua_pushnumber(L, point.y);

    return 2;
}

int Wrap_Graphics::NewFont(lua_State* L)
{
    Font* font = nullptr;

    // Convert to Rasterizer, if necessary.
    if (!Luax::IsType(L, 1, Rasterizer::type))
    {
        std::vector<int> idxs;

        for (int i = 0; i < lua_gettop(L); i++)
            idxs.push_back(i + 1);

        Luax::ConvertObject(L, idxs, "font", "newRasterizer");
    }

    Rasterizer* rasterizer = Luax::CheckType<Rasterizer>(L, 1);

    Luax::CatchException(
        L, [&]() { font = instance()->NewFont(rasterizer, instance()->GetDefaultFilter()); });

    Luax::PushType(L, font);
    font->Release();

    return 1;
}

int Wrap_Graphics::NewQuad(lua_State* L)
{
    Quad::Viewport v;

    v.x = luaL_checknumber(L, 1);
    v.y = luaL_checknumber(L, 2);

    v.w = luaL_checknumber(L, 3);
    v.h = luaL_checknumber(L, 4);

    double sw = 0.0;
    double sh = 0.0;

    if (Luax::IsType(L, 5, Texture::type))
    {
        Texture* texture = Wrap_Texture::CheckTexture(L, 5);

        sw = texture->GetWidth();
        sh = texture->GetHeight();
    }
    else
    {
        sw = luaL_checknumber(L, 5);
        sh = luaL_checknumber(L, 6);
    }

    Quad* quad = instance()->NewQuad(v, sw, sh);

    Luax::PushType(L, quad);
    quad->Release();

    return 1;
}

int Wrap_Graphics::Draw(lua_State* L)
{
    Drawable* drawable = nullptr;
    Texture* texture   = nullptr;
    Quad* quad         = nullptr;

    int start = 2;

    if (Luax::IsType(L, 2, Quad::type))
    {
        texture = Wrap_Texture::CheckTexture(L, 1);
        quad    = Luax::ToType<Quad>(L, 2);

        start = 3;
    }
    else if (lua_isnil(L, 2) && !lua_isnoneornil(L, 3))
        return Luax::TypeErrror(L, 2, "Quad");
    else
    {
        drawable = Luax::CheckType<Drawable>(L, 1);
        start    = 2;
    }

    Graphics::CheckStandardTransform(L, start, [&](const Matrix4& m) {
        Luax::CatchException(L, [&]() {
            if (texture && quad)
                instance()->Draw(texture, quad, m);
            else
                instance()->Draw(drawable, m);
        });
    });

    return 0;
}

int Wrap_Graphics::Print(lua_State* L)
{
    std::vector<Font::ColoredString> string;
    Wrap_Font::CheckColoredString(L, 1, string);

    if (Luax::IsType(L, 2, Font::type))
    {
        Font* font = Wrap_Font::CheckFont(L, 2);

        Graphics::CheckStandardTransform(L, 3, [&](const Matrix4& m) {
            Luax::CatchException(L, [&]() { instance()->Print(string, font, m); });
        });
    }
    else
    {
        Graphics::CheckStandardTransform(L, 2, [&](const Matrix4& m) {
            Luax::CatchException(L, [&]() { instance()->Print(string, m); });
        });
    }

    return 0;
}

int Wrap_Graphics::PrintF(lua_State* L)
{
    std::vector<Font::ColoredString> string;
    Wrap_Font::CheckColoredString(L, 1, string);

    int start = 2;

    Font* font = nullptr;
    if (Luax::IsType(L, start, Font::type))
    {
        font = Wrap_Font::CheckFont(L, start);
        start++;
    }

    Font::AlignMode mode = Font::ALIGN_LEFT;
    Matrix4 m;

    int formatidx = start + 2;

    float wrap = luaL_checknumber(L, formatidx);
    const char* alignment =
        lua_isnoneornil(L, formatidx + 1) ? nullptr : luaL_checkstring(L, formatidx + 1);

    if (alignment != nullptr && !Font::GetConstant(alignment, mode))
        return Luax::EnumError(L, "alignment", Font::GetConstants(mode), alignment);

    float x = luaL_checknumber(L, start + 0);
    float y = luaL_checknumber(L, start + 1);

    float r  = luaL_optnumber(L, start + 4, 0.0f);
    float sx = luaL_optnumber(L, start + 5, 1.0f);
    float sy = luaL_optnumber(L, start + 6, sx);
    float ox = luaL_optnumber(L, start + 7, 0.0f);
    float oy = luaL_optnumber(L, start + 8, 0.0f);
    float kx = luaL_optnumber(L, start + 9, 0.0f);
    float ky = luaL_optnumber(L, start + 10, 0.0f);

    m = Matrix4(x, y, r, sx, sy, ox, oy, kx, ky);

    if (font != nullptr)
    {
        Luax::CatchException(L, [&]() { instance()->PrintF(string, font, wrap, mode, m); });
    }
    else
    {
        Luax::CatchException(L, [&]() { instance()->PrintF(string, wrap, mode, m); });
    }

    return 0;
}

int Wrap_Graphics::SetNewFont(lua_State* L)
{
    int ret    = Wrap_Graphics::NewFont(L);
    Font* font = Luax::CheckType<Font>(L, -1);

    instance()->SetFont(font);

    return ret;
}

int Wrap_Graphics::SetFont(lua_State* L)
{
    Font* font = Luax::CheckType<Font>(L, 1);

    instance()->SetFont(font);

    return 0;
}

int Wrap_Graphics::SetCanvas(lua_State* L)
{
    Canvas* canvas = nullptr;

    if (!lua_isnoneornil(L, 1))
        canvas = Luax::CheckType<Canvas>(L, 1);

    instance()->SetCanvas(canvas);

    return 0;
}

int Wrap_Graphics::GetFont(lua_State* L)
{
    Font* font = nullptr;
    Luax::CatchException(L, [&]() { font = instance()->GetFont(); });

    Luax::PushType(L, font);

    return 1;
}

int Wrap_Graphics::Reset(lua_State* L)
{
    instance()->Reset();

    return 0;
}

int Wrap_Graphics::GetBackgroundColor(lua_State* L)
{
    Colorf background = instance()->GetBackgroundColor();

    lua_pushnumber(L, background.r);
    lua_pushnumber(L, background.g);
    lua_pushnumber(L, background.b);
    lua_pushnumber(L, background.a);

    return 4;
}

int Wrap_Graphics::SetBackgroundColor(lua_State* L)
{
    Colorf background;

    if (lua_istable(L, 1))
    {
        for (int i = 1; i <= 4; i++)
            lua_rawgeti(L, 1, i);

        background.r = luaL_checknumber(L, -4);
        background.g = luaL_checknumber(L, -3);
        background.b = luaL_checknumber(L, -2);
        background.a = luaL_optnumber(L, -1, 1.0f);
    }
    else if (lua_isnumber(L, 1))
    {
        background.r = luaL_checknumber(L, 1);
        background.g = luaL_checknumber(L, 2);
        background.b = luaL_checknumber(L, 3);
        background.a = luaL_optnumber(L, 4, 1.0f);
    }

    instance()->SetBackgroundColor(background);

    return 0;
}

int Wrap_Graphics::GetColor(lua_State* L)
{
    Colorf foreground = instance()->GetColor();

    lua_pushnumber(L, foreground.r);
    lua_pushnumber(L, foreground.g);
    lua_pushnumber(L, foreground.b);
    lua_pushnumber(L, foreground.a);

    return 4;
}

int Wrap_Graphics::SetColor(lua_State* L)
{
    Colorf foreground = { 0.0f, 0.0f, 0.0f, 0.0f };

    if (lua_istable(L, 1))
    {
        for (int i = 1; i <= 4; i++)
            lua_rawgeti(L, 1, i);

        foreground.r = luaL_checknumber(L, -4);
        foreground.g = luaL_checknumber(L, -3);
        foreground.b = luaL_checknumber(L, -2);
        foreground.a = luaL_optnumber(L, -1, 1.0f);
    }
    else if (lua_isnumber(L, 1))
    {
        foreground.r = luaL_checknumber(L, 1);
        foreground.g = luaL_checknumber(L, 2);
        foreground.b = luaL_checknumber(L, 3);
        foreground.a = luaL_optnumber(L, 4, 1.0f);
    }

    instance()->SetColor(foreground);

    return 0;
}

/* Nintendo 3DS */

int Wrap_Graphics::SetBlendFactor(lua_State* L)
{
    float blendFactor = luaL_optnumber(L, 1, 0.0f);

    instance()->SetBlendFactor(blendFactor);

    return 0;
}

int Wrap_Graphics::GetBlendFactor(lua_State* L)
{
    float blendFactor = instance()->GetBlendFactor();

    lua_pushnumber(L, blendFactor);

    return 1;
}

int Wrap_Graphics::Get3D(lua_State* L)
{
#if defined(_3DS)
    auto instance = (love::citro2d::Graphics*)instance();

    lua_pushboolean(L, instance->Get3D());

    return 1;
#endif
    return 0;
}

int Wrap_Graphics::Set3D(lua_State* L)
{
#if defined(_3DS)
    bool enabled = Luax::ToBoolean(L, 1);

    auto instance = (love::citro2d::Graphics*)instance();
    instance->Set3D(enabled);
#endif
    return 0;
}

int Wrap_Graphics::Get3DDepth(lua_State* L)
{
#if defined(_3DS)
    auto instance     = (love::citro2d::Graphics*)instance();
    float sliderValue = (instance->Get3D()) ? osGet3DSliderState() : 0;

    lua_pushnumber(L, sliderValue);

    return 1;
#endif
    return 0;
}

/* End Nintendo 3DS */

int Wrap_Graphics::GetRendererInfo(lua_State* L)
{
    Graphics::RendererInfo info = instance()->GetRendererInfo();

    Luax::PushString(L, info.name);
    Luax::PushString(L, info.version);
    Luax::PushString(L, info.vendor);
    Luax::PushString(L, info.device);

    return 4;
}

int Wrap_Graphics::Register(lua_State* L)
{
    luaL_Reg funcs[] = { { "arc", Arc },
                         { "applyTransform", ApplyTransform },
                         { "circle", Circle },
                         { "clear", Clear },
                         { "draw", Draw },
                         { "ellipse", Ellipse },
                         { "getActiveScreen", GetActiveScreen },
                         { "getBackgroundColor", GetBackgroundColor },
                         { "getColor", GetColor },
                         { "getDefaultFilter", GetDefaultFilter },
                         { "getDimensions", GetDimensions },
                         { "getFont", GetFont },
                         { "getHeight", GetHeight },
                         { "getLineWidth", GetLineWidth },
                         { "getPointSize", GetPointSize },
                         { "getRendererInfo", GetRendererInfo },
                         { "getScissor", GetScissor },
                         { "getScreens", GetScreens },
                         { "getWidth", GetWidth },
                         { "instersectScissor", IntersectScissor },
                         { "inverseTransformPoint", InverseTransformPoint },
                         { "line", Line },
                         { "newCanvas", NewCanvas },
                         { "newFont", NewFont },
                         { "newImage", NewImage },
                         { "newText", NewText },
                         { "newQuad", NewQuad },
                         { "origin", Origin },
                         { "polygon", Polygon },
                         { "pop", Pop },
                         { "points", Points },
                         { "present", Present },
                         { "print", Print },
                         { "printf", PrintF },
                         { "push", Push },
                         { "rectangle", Rectangle },
                         { "replaceTransform", ReplaceTransform },
                         { "reset", Reset },
                         { "rotate", Rotate },
                         { "scale", Scale },
                         { "shear", Shear },
                         { "setActiveScreen", SetActiveScreen },
                         { "setBackgroundColor", SetBackgroundColor },
                         { "setCanvas", SetCanvas },
                         { "setColor", SetColor },
                         { "setDefaultFilter", SetDefaultFilter },
                         { "setLineWidth", SetLineWidth },
                         { "setNewFont", SetNewFont },
                         { "setPointSize", SetPointSize },
                         { "setFont", SetFont },
                         { "setScissor", SetScissor },
                         { "transformPoint", TransformPoint },
                         { "translate", Translate },
                         { 0, 0 } };

    /* 3DS extensions */

    luaL_Reg modExt[] = { { "getBlendFactor", GetBlendFactor },
                          { "get3D", Get3D },
                          { "get3DDepth", Get3DDepth },
                          { "setBlendFactor", SetBlendFactor },
                          { "set3D", Set3D } };

    /* if it doesn't match the console, it just copies input to be the output */

    std::unique_ptr<luaL_Reg[]> reg = Luax::ExtendIf("3DS", funcs, modExt);

    /* objects */

    lua_CFunction objs[] = {
        Wrap_Drawable::Register,
        Wrap_Texture::Register,
        Wrap_Font::Register,
        Wrap_Image::Register,
        Wrap_Quad::Register,
        Wrap_Canvas::Register,
        Wrap_Text::Register,
#if defined(__SWITCH__)
        Wrap_Shader::Register,
#endif
        0
    };

    Graphics* instance = instance();

    if (instance == nullptr)
#if defined(_3DS)
        Luax::CatchException(L, [&]() { instance = new love::citro2d::Graphics(); });
#elif defined(__SWITCH__)
        Luax::CatchException(L, [&]() { instance = new love::deko3d::Graphics(); });
#endif
    else
        instance->Retain();

    WrappedModule wrappedModule;

    wrappedModule.instance  = instance;
    wrappedModule.name      = "graphics";
    wrappedModule.functions = reg.get();
    wrappedModule.type      = &Module::type;
    wrappedModule.types     = objs;

    int result = Luax::RegisterModule(L, wrappedModule);

    return result;
}
