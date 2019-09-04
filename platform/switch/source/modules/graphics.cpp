#include "common/runtime.h"

#include "modules/graphics.h"

#include "modules/window.h"
#include "modules/display.h"

#include "common/drawable.h"
#include "objects/image/image.h"
#include "objects/image/wrap_image.h"

#include "objects/quad/quad.h"
#include "objects/quad/wrap_quad.h"

#include "objects/font/font.h"
#include "objects/font/wrap_font.h"

#include "objects/canvas/canvas.h"
#include "objects/canvas/wrap_canvas.h"

#include "objects/text/text.h"
#include "objects/text/wrap_text.h"

Font * currentFont = NULL;

vector<StackMatrix> stack;
template <typename T>
void transformDrawable(T * originalX, T * originalY) // rotate, scale, and translate coords.
{
    if (stack.empty())
        return;

    StackMatrix & matrix = stack.back();

    //float newLeft = *originalX;
    //float newTop = *originalY;

    //Translate
    *originalX += (T) matrix.ox;
    *originalY += (T) matrix.oy;

    //Scale
    /*originalX *= matrix.sx;
    *originalY *= matrix.sy;

    //Rotate
    *originalX = newLeft * cos(matrix.r) - newTop * sin(matrix.r);
    *originalY = newLeft * sin(matrix.r) + newTop * cos(matrix.r);

    //Shear
    *originalX = newLeft + matrix.kx * newTop;
    *originalY = newTop + matrix.ky * newLeft;*/
}

void Graphics::Initialize()
{

}

//Löve2D Functions

//love.graphics.getDimensions
int Graphics::GetDimensions(lua_State * L)
{
    lua_pushnumber(L, 1280);
    lua_pushnumber(L, 720);

    return 2;
}

//love.graphics.getWidth
int Graphics::GetWidth(lua_State * L)
{
    lua_pushnumber(L, 1280);

    return 1;
}

//love.graphics.getHeight
int Graphics::GetHeight(lua_State * L)
{
    lua_pushnumber(L, 720);

    return 1;
}

//love.graphics.setBackgroundColor
int Graphics::SetBackgroundColor(lua_State * L)
{
    float r = backgroundColor.r;
    float g = backgroundColor.g;
    float b = backgroundColor.b;

    if (lua_isnumber(L, 1))
    {
        r = clamp(0, luaL_checknumber(L, 1), 1);
        g = clamp(0, luaL_checknumber(L, 2), 1);
        b = clamp(0, luaL_checknumber(L, 3), 1);
    }
    else if (lua_istable(L, 1))
    {
        for (int i = 1; i <= 4; i++)
            lua_rawgeti(L, 1, i);

        r = clamp(0, luaL_checknumber(L, -4), 1);
        g = clamp(0, luaL_checknumber(L, -3), 1);
        b = clamp(0, luaL_checknumber(L, -2), 1);
    }

    backgroundColor.r = r * 255;
    backgroundColor.g = g * 255;
    backgroundColor.b = b * 255;

    return 0;
}

//love.graphics.setColor
int Graphics::SetColor(lua_State * L)
{
    float r = drawColor.r;
    float g = drawColor.g;
    float b = drawColor.b;
    float a = drawColor.a;

    if (lua_isnumber(L, 1))
    {
        r = clamp(0, luaL_checknumber(L, 1), 1);
        g = clamp(0, luaL_checknumber(L, 2), 1);
        b = clamp(0, luaL_checknumber(L, 3), 1);
        a = clamp(0, luaL_optnumber(L, 4, 1), 1);
    }
    else if (lua_istable(L, 1))
    {
        for (int i = 1; i <= 4; i++)
            lua_rawgeti(L, 1, i);

        r = clamp(0, luaL_checknumber(L, -4), 1);
        g = clamp(0, luaL_checknumber(L, -3), 1);
        b = clamp(0, luaL_checknumber(L, -2), 1);
        a = clamp(0, luaL_optnumber(L, -1, 1), 1);
    }

    drawColor.r = r * 255;
    drawColor.g = g * 255;
    drawColor.b = b * 255;
    drawColor.a = a * 255;

    SDL_SetRenderDrawColor(Display::GetRenderer(), drawColor.r, drawColor.g, drawColor.b, drawColor.a);

    return 0;
}

//love.graphics.getBackgroundColor
int Graphics::GetBackgroundColor(lua_State * L)
{
    lua_pushnumber(L, backgroundColor.r / 255.0f);
    lua_pushnumber(L, backgroundColor.g / 255.0f);
    lua_pushnumber(L, backgroundColor.b / 255.0f);

    return 3;
}

//love.graphics.getColor
int Graphics::GetColor(lua_State * L)
{
    lua_pushnumber(L, drawColor.r / 255.0f);
    lua_pushnumber(L, drawColor.g / 255.0f);
    lua_pushnumber(L, drawColor.b / 255.0f);
    lua_pushnumber(L, drawColor.a / 255.0f);

    return 4;
}

//love.graphics.clear
int Graphics::Clear(lua_State * L)
{
    SDL_SetRenderDrawColor(Display::GetRenderer(), backgroundColor.r, backgroundColor.g, backgroundColor.b, 0);
    SDL_RenderClear(Display::GetRenderer());

    SDL_SetRenderDrawColor(Display::GetRenderer(), drawColor.r, drawColor.g, drawColor.b, drawColor.a);

    return 0;
}

//love.graphics.present
int Graphics::Present(lua_State * L)
{
    SDL_RenderPresent(Display::GetRenderer());

    return 0;
}

//love.graphics.draw
int Graphics::Draw(lua_State * L)
{
    Drawable * drawable = nullptr;
    Quad * quad = nullptr;

    if (luaobj_type(L, 1, LUAOBJ_TYPE_IMAGE))
        drawable = (Image *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_IMAGE);
    else if (luaobj_type(L, 1, LUAOBJ_TYPE_CANVAS))
        drawable = (Canvas *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_CANVAS);
    else if (luaobj_type(L, 1, LUAOBJ_TYPE_TEXT))
        drawable = (Text *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_TEXT);
    else
        return luaL_error(L, "bad argument #%d, %s expected, got %s.", 1, "Drawable", luax_stype(L, 1, NULL));

    int start = 2;
    if (!lua_isnoneornil(L, 2) && !lua_isnumber(L, 2))
    {
        quad = (Quad *)luaobj_checkudata(L, 2, LUAOBJ_TYPE_QUAD);
        start = 3;
    }

    float x = luaL_optnumber(L, start + 0, 0);
    float y = luaL_optnumber(L, start + 1, 0);

    float rotation = luaL_optnumber(L, start + 2, 0);

    float scalarX = luaL_optnumber(L, start + 3, 1);
    float scalarY = luaL_optnumber(L, start + 4, scalarX);

    float offsetX = luaL_optnumber(L, start + 5, 0);
    float offsetY = luaL_optnumber(L, start + 6, 0);

    transformDrawable(&x, &y);

    rotation *= 180 / M_PI;

    if (quad != nullptr)
        drawable->Draw(NULL, quad->GetViewport(), x, y, rotation, offsetX, offsetY, scalarX, scalarY, drawColor);
    else
        drawable->Draw(NULL, drawable->GetViewport(), x, y, rotation, offsetX, offsetY, scalarX, scalarY, drawColor);

    return 0;
}

int Graphics::SetDefaultFilter(lua_State * L)
{
    string filter = luaL_checkstring(L, 1);
    string quality = "1";

    if (filter == "nearest")
        quality = "0";

    const char * c_quality = quality.c_str();
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, c_quality);

    return 0;
}

//love.graphics.getFont
int Graphics::GetFont(lua_State * L)
{
    love_push_userdata(L, currentFont);

    return 1;
}

//love.graphics.setFont
int Graphics::SetFont(lua_State * L)
{
    Font * self = (Font *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_FONT);

    if (self != NULL)
        currentFont = self;

    return 0;
}

//love.graphics.setNewFont
int Graphics::SetNewFont(lua_State * L)
{
    int ret = fontNew(L);
    Font * self = (Font *)luaobj_checkudata(L, -1, LUAOBJ_TYPE_FONT);
    currentFont = self;

    return ret;
}

//love.graphics.print
int Graphics::Print(lua_State * L)
{
    const char * text = luaL_checkstring(L, 1);

    float x = luaL_optnumber(L, 2, 0);
    float y = luaL_optnumber(L, 3, 0);

    float rotation = luaL_optnumber(L, 4, 0);

    float scalarX = luaL_optnumber(L, 5, 1);
    float scalarY = luaL_optnumber(L, 6, 1);

    float offsetX = luaL_optnumber(L, 7, 0);
    float offsetY = luaL_optnumber(L, 8, 0);

    transformDrawable(&x, &y);

    rotation *= 180 / M_PI;

    if (currentFont == NULL)
        return 0;

    currentFont->Print(text, x, y, rotation, 1280, offsetX, offsetY, scalarX, scalarY, drawColor);

    return 0;
}

//love.graphics.printf
int Graphics::Printf(lua_State * L)
{
    const char * text = luaL_checkstring(L, 1);

    float x = luaL_optnumber(L, 2, 0);
    float y = luaL_optnumber(L, 3, 0);

    float limit = luaL_optnumber(L, 4, 1280);

    string align = luaL_optstring(L, 5, "left");

    float rotation = luaL_optnumber(L, 6, 0);

    float scalarX = luaL_optnumber(L, 7, 1);
    float scalarY = luaL_optnumber(L, 8, 1);

    float offsetX = luaL_optnumber(L, 9, 0);
    float offsetY = luaL_optnumber(L, 10, 0);

    transformDrawable(&x, &y);

    rotation *= 180 / M_PI;

    x -= offsetX;
    y -= offsetY;

    if (currentFont == NULL)
        return 0;

    if (align == "center")
        x += (limit / 2);
    else if (align == "right")
        x += limit;

    currentFont->Print(text, x, y, rotation, limit, offsetX, offsetY, scalarX, scalarY, drawColor);

    return 0;
}

//love.graphics.setCanvas
int Graphics::SetCanvas(lua_State * L)
{
    Canvas * self = NULL;
    if (!lua_isnoneornil(L, 1))
        self = (Canvas *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_CANVAS);

    if (self != NULL)
        self->SetAsTarget();
    else
        SDL_SetRenderTarget(Display::GetRenderer(), NULL);

    return 0;
}

//love.graphics.rectangle
int Graphics::Rectangle(lua_State * L)
{
    string mode = luaL_checkstring(L, 1);

    LOVE_VALIDATE_DRAW_MODE(mode.c_str());

    float x = luaL_optnumber(L, 2, 0);
    float y = luaL_optnumber(L, 3, 0);

    float width = luaL_checknumber(L, 4);
    float height = luaL_checknumber(L, 5);

    float cornerRadius = luaL_optnumber(L, 6, 0);

    transformDrawable(&x, &y);

    if (mode == "fill")
        roundedBoxRGBA(Display::GetRenderer(), roundf(x), roundf(y), roundf(x + width - 1), roundf(y + height - 1), cornerRadius, drawColor.r, drawColor.g, drawColor.b, drawColor.a);
    else if (mode == "line")
        roundedRectangleRGBA(Display::GetRenderer(), roundf(x), roundf(y), x + width - 1, y + height - 1, cornerRadius, drawColor.r, drawColor.g, drawColor.b, drawColor.a);

    return 0;
}

//love.graphics.polygon
int Graphics::Polygon(lua_State * L)
{
    int args = lua_gettop(L) - 1; // vertices count (or a table)

    string mode = luaL_checkstring(L, 1);

    LOVE_VALIDATE_DRAW_MODE(mode.c_str());

    // check if the user provided a table instead of giant list
    bool isTable = false;
    if (args == 1 && lua_istable(L, 2))
    {
        args = (int) lua_objlen(L, 2);
        isTable = true;
    }

    // throw errors if theres a problem
    if (args % 2 != 0)
        return luaL_error(L, "Number of vertex components must be a multiple of two");
    else if (args < 6)
        return luaL_error(L, "Need at least three vertices to draw a polygon");

    // gather all the vertices
    int numvertices = args / 2;

    s16 xCoords[numvertices + 1];
    s16 yCoords[numvertices + 1];

    if (isTable)
    {
        // temporary storage of the coordinates for transforming
        s16 x = 0, y = 0;

        for (int i = 0; i < numvertices; i++)
        {
            lua_rawgeti(L, 2, (i * 2) + 1);
            lua_rawgeti(L, 2, (i * 2) + 2);

            x = luaL_checkinteger(L, -2);
            y = luaL_checkinteger(L, -1);

            transformDrawable(&x, &y);

            xCoords[i] = x;
            yCoords[i] = y;

            lua_pop(L, 2);
        }
    }
    else
    {
        // same here
        s16 x = 0, y = 0;

        for (int i = 0; i < numvertices; i++)
        {
            x = luaL_checkinteger(L, (i * 2) + 2);
            y = luaL_checkinteger(L, (i * 2) + 3);

            transformDrawable(&x, &y);

            xCoords[i] = x;
            yCoords[i] = y;

            lua_pop(L, 2);
        }
    }

    // close the loop
    xCoords[numvertices] = xCoords[0];
    yCoords[numvertices] = yCoords[0];

    // finally draw
    if (mode == "fill")
        filledPolygonRGBA(Display::GetRenderer(), xCoords, yCoords, numvertices, drawColor.r, drawColor.g, drawColor.b, drawColor.a);
    else if (mode == "line")
        polygonRGBA(Display::GetRenderer(), xCoords, yCoords, numvertices, drawColor.r, drawColor.g, drawColor.b, drawColor.a);

    return 0;
}

//love.graphics.arc
int Graphics::Arc(lua_State * L)
{
    string mode = luaL_checkstring(L, 1);

    LOVE_VALIDATE_DRAW_MODE(mode.c_str());

    float x = luaL_optnumber(L, 2, 0);
    float y = luaL_optnumber(L, 3, 0);

    float radius = luaL_checknumber(L, 4);

    float start = luaL_checknumber(L, 5);
    float end = luaL_checknumber(L, 6);

    start *= 180 / M_PI;
    end *= 180 / M_PI;

    transformDrawable(&x, &y);

    if (mode == "line")
        pieRGBA(Display::GetRenderer(), x, y, radius, start, end, drawColor.r, drawColor.g, drawColor.b, drawColor.a);
    else if (mode == "fill")
        filledPieRGBA(Display::GetRenderer(), x, y, radius, start, end, drawColor.r, drawColor.g, drawColor.b, drawColor.a);

    return 0;
}

//love.graphics.circle
int Graphics::Circle(lua_State * L)
{
    string mode = luaL_checkstring(L, 1);

    LOVE_VALIDATE_DRAW_MODE(mode.c_str());

    float x = luaL_optnumber(L, 2, 0);
    float y = luaL_optnumber(L, 3, 0);

    float radius = luaL_checknumber(L, 4);

    transformDrawable(&x, &y);

    if (mode == "fill")
        filledCircleRGBA(Display::GetRenderer(), x, y, radius, drawColor.r, drawColor.g, drawColor.b, drawColor.a);
    else if (mode == "line")
        circleRGBA(Display::GetRenderer(), x, y, radius, drawColor.r, drawColor.g, drawColor.b, drawColor.a);

    return 0;
}

//love.graphics.setLineWidth
int Graphics::SetLineWidth(lua_State * L)
{
    lineWidth = luaL_checknumber(L, 1);

    return 0;
}

//love.graphics.getLineWidth
int Graphics::GetLineWidth(lua_State * L)
{
    lua_pushnumber(L, lineWidth);

    return 1;
}

//love.graphics.line
int Graphics::Line(lua_State * L)
{
    int argc = lua_gettop(L);
    bool isTable = lua_istable(L, 1);
    int tableLength = 0;

    float startx = 0, starty = 0, endx = 0, endy = 0;

    if (isTable)
    {
        tableLength = lua_objlen(L, 1);

        if (tableLength == 0 || (tableLength > 0 && (tableLength % 4) != 0))
            return luaL_error(L, "%s", "Need at least two verticies to draw a line");

        if ((tableLength % 4) == 0)
        {
            for (int j = 0; j < tableLength; j += 4)
            {
                for (int i = 1; i <= 4; i++)
                    lua_rawgeti(L, 1, i + j);

                startx = luaL_checknumber(L, -4);
                starty = luaL_checknumber(L, -3);

                endx = luaL_checknumber(L, -2);
                endy = luaL_checknumber(L, -1);

                lua_pop(L, 4);

                transformDrawable(&startx, &starty);
                transformDrawable(&endx, &endy);

                thickLineRGBA(Display::GetRenderer(), startx, starty, endx, endy, lineWidth, drawColor.r, drawColor.g, drawColor.b, drawColor.a);
            }
        }
    }
    else
    {
        if ((argc % 4) != 0)
            return luaL_error(L, "%s", "Need at least two verticies to draw a line");

        for (int i = 0; i < argc; i += 4)
        {
            startx = luaL_checknumber(L, i + 1);
            starty = luaL_checknumber(L, i + 2);

            endx = luaL_checknumber(L, i + 3);
            endy = luaL_checknumber(L, i + 4);

            transformDrawable(&startx, &starty);
            transformDrawable(&endx, &endy);

            thickLineRGBA(Display::GetRenderer(), startx, starty, endx, endy, lineWidth, drawColor.r, drawColor.g, drawColor.b, drawColor.a);
        }
    }

    return 0;
}

//love.graphics.points
int Graphics::Points(lua_State * L)
{
    int args = lua_gettop(L);

    bool isTable = false;
    bool tableOfTables = false;

    if (args == 1 && lua_istable(L, 1))
    {
        isTable = true;
        args = lua_objlen(L, 1);

        lua_rawgeti(L, 1, 1);
        tableOfTables = lua_istable(L, -1);
        lua_pop(L, 1);
    }

    if (args % 2 != 0 && !tableOfTables)
        return luaL_error(L, "Points must be a multiple of two");

    int pointCount = args / 2;
    if (tableOfTables)
        pointCount = args;

    SDL_Point points[pointCount];

    float x;
    float y;

    if (isTable)
    {
        if (!tableOfTables)
        {
            for (int i = 0; i < pointCount; i++)
            {
                lua_rawgeti(L, 1, i * 2 + 1);
                lua_rawgeti(L, 1, i * 2 + 2);

                x = luaL_checknumber(L, -2);
                y = luaL_checknumber(L, -1);

                transformDrawable(&x, &y);

                points[i].x = x;
                points[i].y = y;

                lua_pop(L, 2);
            }
        }
        else
        {
            for (int i = 0; i < args; i++)
            {
                lua_rawgeti(L, 1, i + 1);

                for (int j = 1; j <= 2; j++) // <= 6 for r,g,b,a functionality
                    lua_rawgeti(L, -j, j);

                x = luaL_checknumber(L, -2);
                y = luaL_checknumber(L, -1);

                transformDrawable(&x, &y);

                points[i].x = x;
                points[i].y = y;

                lua_pop(L, 3);
            }
        }
    }
    else
    {
        for (int i = 0; i < pointCount; i++)
        {
            x = luaL_checknumber(L, i * 2 + 1);
            y = luaL_checknumber(L, i * 2 + 2);

            transformDrawable(&x, &y);

            points[i].x = x;
            points[i].y = y;
        }
    }

    SDL_RenderDrawPoints(Display::GetRenderer(), points, args);

    return 0;
}

//love.graphics.setScissor
int Graphics::SetScissor(lua_State * L)
{
    if (lua_isnoneornil(L, 1))
        SDL_RenderSetClipRect(Display::GetRenderer(), NULL);
    else
    {
        int x = luaL_checknumber(L, 1);
        int y = luaL_checknumber(L, 2);

        int width = luaL_checknumber(L, 3);
        int height = luaL_checknumber(L, 4);

        if (width < 0 || height < 0)
            return luaL_error(L, "Scissor cannot have a negative width or height");

        SDL_Rect scissor = {x, y, width, height};

        SDL_RenderSetClipRect(Display::GetRenderer(), &scissor);
    }

    return 0;
}

//love.graphics.push
int Graphics::Push(lua_State * L)
{
    stack.push_back(stack.back());

    return 0;
}

//love.graphics.translate
int Graphics::Translate(lua_State * L)
{
    float x = luaL_checknumber(L, 1);
    float y = luaL_checknumber(L, 2);

    stack.back().ox += x;
    stack.back().oy += y;

    return 0;
}

//love.graphics.scale
int Graphics::Scale(lua_State * L)
{
    float sx = luaL_checknumber(L, 1);
    float sy = luaL_checknumber(L, 2);

    stack.back().sx = sx;
    stack.back().sy = sy;

    return 0;
}

//love.graphics.rotate
int Graphics::Rotate(lua_State * L)
{
    float r = luaL_checknumber(L, 1);

    stack.back().r = r;

    return 0;
}

//love.graphics.shear
int Graphics::Shear(lua_State * L)
{
    float kx = luaL_checknumber(L, 1);
    float ky = luaL_checknumber(L, 2);

    stack.back().kx = kx;
    stack.back().ky = ky;

    return 0;
}

//love.graphics.origin
int Graphics::Origin(lua_State * L)
{
    stack.back().ox = 0;
    stack.back().oy = 0;

    stack.back().sx = 1;
    stack.back().sy = 1;

    stack.back().r = 0;

    stack.back().kx = 0;
    stack.back().ky = 0;

    return 0;
}

//love.graphics.pop
int Graphics::Pop(lua_State * L)
{
    stack.pop_back();

    return 0;
}

//love.graphics.getRendererInfo
int Graphics::GetRendererInfo(lua_State * L)
{
    lua_pushstring(L, "OpenGL");
    lua_pushstring(L, "4.5");
    lua_pushstring(L, "NVIDIA");
    lua_pushstring(L, "Tegra X1");

    return 4;
}

//End Löve2D Functions

void Graphics::Exit()
{
    TTF_Quit();
    SDL_Quit(); //kill remaining stuff
}

int Graphics::Register(lua_State * L)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        luaL_error(L, "Failed to initialize SDL2!");

    TTF_Init();

    stack.reserve(16);
    stack.push_back(StackMatrix());

    luaL_Reg reg[] =
    {
        { "arc",                Arc                },
        { "circle",             Circle             },
        { "clear",              Clear              },
        { "draw",               Draw               },
        { "getBackgroundColor", GetBackgroundColor },
        { "getColor",           GetColor           },
        { "getDimensions",      GetDimensions      },
        { "getFont",            GetFont            },
        { "getHeight",          GetHeight          },
        { "getLineWidth",       GetLineWidth       },
        { "getRendererInfo",    GetRendererInfo    },
        { "getWidth",           GetWidth           },
        { "line",               Line               },
        { "newCanvas",          canvasNew          },
        { "newFont",            fontNew            },
        { "newImage",           imageNew           },
        { "newQuad",            quadNew            },
        { "newText",            textNew            },
        { "origin",             Origin             },
        { "points",             Points             },
        { "pop",                Pop                },
        { "present",            Present            },
        { "print",              Print              },
        { "printf",             Printf             },
        { "push",               Push               },
        { "rectangle",          Rectangle          },
        { "polygon",            Polygon            },
        { "rotate",             Rotate             },
        { "scale",              Scale              },
        { "setBackgroundColor", SetBackgroundColor },
        { "setCanvas",          SetCanvas          },
        { "setColor",           SetColor           },
        { "setDefaultFilter",   SetDefaultFilter   },
        { "setFont",            SetFont            },
        { "setLineWidth",       SetLineWidth       },
        { "setNewFont",         SetNewFont         },
        { "setScissor",         SetScissor         },
        { "shear",              Shear              },
        { "translate",          Translate          },
        { 0, 0 }
    };

    luaL_newlib(L, reg);

    return 1;
}
