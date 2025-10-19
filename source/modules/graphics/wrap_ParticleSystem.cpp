#include "common/Vector.hpp"

#include "modules/graphics/Texture.tcc"
#include "modules/graphics/wrap_ParticleSystem.hpp"
#include "modules/graphics/wrap_Texture.hpp"

#include <cstring>

using namespace love;

int Wrap_ParticleSystem::clone(lua_State* L)
{
    auto* self            = luax_checkparticlesystem(L, 1);
    ParticleSystem* clone = nullptr;

    luax_catchexcept(L, [&]() { clone = self->clone(); });

    luax_pushtype(L, clone);
    clone->release();
    return 1;
}

int Wrap_ParticleSystem::setTexture(lua_State* L)
{
    auto* self    = luax_checkparticlesystem(L, 1);
    auto* texture = luax_checktexture(L, 2);

    luax_catchexcept(L, [&]() { self->setTexture(texture); });

    return 0;
}

int Wrap_ParticleSystem::getTexture(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);

    luax_pushtype(L, self->getTexture());

    return 1;
}

int Wrap_ParticleSystem::setBufferSize(lua_State* L)
{
    auto* self      = luax_checkparticlesystem(L, 1);
    lua_Number size = luaL_checknumber(L, 2);

    if (size < 1.0 || size > ParticleSystem::MAX_PARTICLES)
        return luaL_error(L, "Invalid buffer size.");

    luax_catchexcept(L, [&]() { self->setBufferSize((uint32_t)size); });

    return 0;
}

int Wrap_ParticleSystem::getBufferSize(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);
    auto size  = self->getBufferSize();

    lua_pushinteger(L, size);

    return 1;
}

int Wrap_ParticleSystem::setInsertMode(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);

    ParticleSystem::InsertMode mode;
    const char* string = luaL_checkstring(L, 2);

    if (!ParticleSystem::getConstant(string, mode))
        return luax_enumerror(L, "insert mode", ParticleSystem::InsertModes, string);

    self->setInsertMode(mode);

    return 0;
}

int Wrap_ParticleSystem::getInsertMode(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);

    ParticleSystem::InsertMode mode = self->getInsertMode();
    std::string_view string {};
    if (!ParticleSystem::getConstant(mode, string))
        return luaL_error(L, "Unknown insert mode.");

    luax_pushstring(L, string);

    return 1;
}

int Wrap_ParticleSystem::setEmissionRate(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);
    float rate = luaL_checknumber(L, 2);

    luax_catchexcept(L, [&]() { self->setEmissionRate(rate); });

    return 0;
}

int Wrap_ParticleSystem::getEmissionRate(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);
    auto rate  = self->getEmissionRate();

    lua_pushnumber(L, rate);

    return 0;
}

int Wrap_ParticleSystem::setEmitterLifetime(lua_State* L)
{
    auto* self     = luax_checkparticlesystem(L, 1);
    float lifetime = luaL_checknumber(L, 2);

    self->setEmitterLifetime(lifetime);

    return 0;
}

int Wrap_ParticleSystem::getEmitterLifetime(lua_State* L)
{
    auto* self     = luax_checkparticlesystem(L, 1);
    float lifetime = self->getEmitterLifetime();

    lua_pushnumber(L, lifetime);

    return 1;
}

int Wrap_ParticleSystem::setParticleLifetime(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);
    float min  = luaL_checknumber(L, 2);
    float max  = luaL_optnumber(L, 3, min);

    if (min < 0.0f || max < 0.0f)
        return luaL_error(L, "Invalid particle lifetime (must be >= 0).");

    self->setParticleLifetime(min, max);

    return 0;
}

int Wrap_ParticleSystem::getParticleLifetime(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);
    float min, max;

    self->getParticleLifetime(min, max);

    lua_pushnumber(L, min);
    lua_pushnumber(L, max);

    return 2;
}

int Wrap_ParticleSystem::setPosition(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);
    float x    = luaL_checknumber(L, 2);
    float y    = luaL_checknumber(L, 3);

    self->setPosition(x, y);

    return 0;
}

int Wrap_ParticleSystem::getPosition(lua_State* L)
{
    auto* self       = luax_checkparticlesystem(L, 1);
    Vector2 position = self->getPosition();

    lua_pushnumber(L, position.x);
    lua_pushnumber(L, position.y);

    return 2;
}

int Wrap_ParticleSystem::moveTo(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);
    float x    = luaL_checknumber(L, 2);
    float y    = luaL_checknumber(L, 3);

    self->moveTo(x, y);

    return 0;
}

int Wrap_ParticleSystem::setEmissionArea(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);

    auto distribution = ParticleSystem::DISTRIBUTION_NONE;
    float x = 0.0f, y = 0.0f;
    float angle                    = 0.0f;
    bool directionRelativeToCenter = false;

    const char* string = lua_isnoneornil(L, 2) ? nullptr : luaL_checkstring(L, 2);
    if (string && !ParticleSystem::getConstant(string, distribution))
        return luax_enumerror(L, "particle distribution", ParticleSystem::AreaSpreadDistributions, string);

    if (distribution != ParticleSystem::DISTRIBUTION_NONE)
    {
        x = luaL_checknumber(L, 3);
        y = luaL_checknumber(L, 4);

        if (x < 0.0f || y < 0.0f)
            return luaL_error(L, "Invalid area spread parameters (must be >= 0).");

        angle                     = luaL_optnumber(L, 5, 0.0f);
        directionRelativeToCenter = luax_optboolean(L, 6, false);
    }

    self->setEmissionArea(distribution, x, y, angle, directionRelativeToCenter);

    return 0;
}

int Wrap_ParticleSystem::getEmissionArea(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);

    Vector2 params {};
    float angle                    = 0.0f;
    bool directionRelativeToCenter = false;
    auto distribution              = self->getEmissionArea(params, angle, directionRelativeToCenter);

    std::string_view string {};
    if (!ParticleSystem::getConstant(distribution, string))
        return luaL_error(L, "Unknown distribution type.");

    luax_pushstring(L, string);
    lua_pushnumber(L, params.x);
    lua_pushnumber(L, params.y);
    lua_pushnumber(L, angle);
    luax_pushboolean(L, directionRelativeToCenter);

    return 5;
}

int Wrap_ParticleSystem::setDirection(lua_State* L)
{
    auto* self      = luax_checkparticlesystem(L, 1);
    float direction = luaL_checknumber(L, 2);

    self->setDirection(direction);

    return 0;
}

int Wrap_ParticleSystem::getDirection(lua_State* L)
{
    auto* self      = luax_checkparticlesystem(L, 1);
    float direction = self->getDirection();

    lua_pushnumber(L, direction);

    return 1;
}

int Wrap_ParticleSystem::setSpread(lua_State* L)
{
    auto* self   = luax_checkparticlesystem(L, 1);
    float spread = luaL_checknumber(L, 2);

    self->setSpread(spread);

    return 0;
}

int Wrap_ParticleSystem::getSpread(lua_State* L)
{
    auto* self   = luax_checkparticlesystem(L, 1);
    float spread = self->getSpread();

    lua_pushnumber(L, spread);

    return 1;
}

int Wrap_ParticleSystem::setSpeed(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);
    float min  = luaL_checknumber(L, 2);
    float max  = luaL_optnumber(L, 3, min);

    self->setSpeed(min, max);

    return 0;
}

int Wrap_ParticleSystem::getSpeed(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);

    float min, max;
    self->getSpeed(min, max);

    lua_pushnumber(L, min);
    lua_pushnumber(L, max);

    return 2;
}

int Wrap_ParticleSystem::setLinearAcceleration(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);
    float xmin = luaL_checknumber(L, 2);
    float ymin = luaL_checknumber(L, 3);
    float xmax = luaL_optnumber(L, 4, xmin);
    float ymax = luaL_optnumber(L, 5, ymin);

    self->setLinearAcceleration(xmin, ymin, xmax, ymax);

    return 0;
}

int Wrap_ParticleSystem::getLinearAcceleration(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);

    Vector2 min, max;
    self->getLinearAcceleration(min, max);

    lua_pushnumber(L, min.x);
    lua_pushnumber(L, min.y);
    lua_pushnumber(L, max.x);
    lua_pushnumber(L, max.y);

    return 4;
}

int Wrap_ParticleSystem::setRadialAcceleration(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);
    float min  = luaL_checknumber(L, 2);
    float max  = luaL_checknumber(L, 3);

    self->setRadialAcceleration(min, max);

    return 0;
}

int Wrap_ParticleSystem::getRadialAcceleration(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);

    float min, max;
    self->getRadialAcceleration(min, max);

    lua_pushnumber(L, min);
    lua_pushnumber(L, max);

    return 2;
}

int Wrap_ParticleSystem::setTangentialAcceleration(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);
    float min  = luaL_checknumber(L, 2);
    float max  = luaL_checknumber(L, 3);

    self->setTangentialAcceleration(min, max);

    return 0;
}

int Wrap_ParticleSystem::getTangentialAcceleration(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);

    float min, max;
    self->getTangentialAcceleration(min, max);

    lua_pushnumber(L, min);
    lua_pushnumber(L, max);

    return 2;
}

int Wrap_ParticleSystem::setLinearDamping(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);
    float min  = luaL_checknumber(L, 2);
    float max  = luaL_checknumber(L, 3);

    self->setLinearDamping(min, max);

    return 0;
}

int Wrap_ParticleSystem::getLinearDamping(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);

    float min, max;
    self->getLinearDamping(min, max);

    lua_pushnumber(L, min);
    lua_pushnumber(L, max);

    return 2;
}

int Wrap_ParticleSystem::setSizes(lua_State* L)
{
    auto* self   = luax_checkparticlesystem(L, 1);
    size_t count = lua_gettop(L) - 1;

    if (count > 8)
        return luaL_error(L, "At most eight (8) sizes may be used.");

    if (count <= 1)
    {
        float size = luax_checkfloat(L, 2);
        self->setSize(size);
    }
    else
    {
        std::vector<float> sizes(count);
        for (size_t index = 0; index < count; ++index)
            sizes[index] = luax_checkfloat(L, (int)(1 + index + 1));

        self->setSizes(sizes);
    }

    return 0;
}

int Wrap_ParticleSystem::getSizes(lua_State* L)
{
    auto* self        = luax_checkparticlesystem(L, 1);
    const auto& sizes = self->getSizes();

    for (size_t index = 0; index < sizes.size(); index++)
        lua_pushnumber(L, sizes[index]);

    return (int)sizes.size();
}

int Wrap_ParticleSystem::setSizeVariation(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);
    float size = luaL_checknumber(L, 2);

    if (size < 0.0f || size > 1.0f)
        return luaL_error(L, "Size variation must be between 0 and 1, inclusive.");

    self->setSizeVariation(size);

    return 0;
}

int Wrap_ParticleSystem::getSizeVariation(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);
    float size = self->getSizeVariation();

    lua_pushnumber(L, self->getSizeVariation());

    return 1;
}

int Wrap_ParticleSystem::setRotation(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);
    float min  = luaL_checknumber(L, 2);
    float max  = luaL_optnumber(L, 3, min);

    self->setRotation(min, max);

    return 0;
}

int Wrap_ParticleSystem::getRotation(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);

    float min, max;
    self->getRotation(min, max);

    lua_pushnumber(L, min);
    lua_pushnumber(L, max);

    return 2;
}

int Wrap_ParticleSystem::setSpin(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);
    float min  = luaL_checknumber(L, 2);
    float max  = luaL_optnumber(L, 3, min);

    self->setSpin(min, max);

    return 0;
}

int Wrap_ParticleSystem::getSpin(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);

    float start, end;
    self->getSpin(start, end);

    lua_pushnumber(L, start);
    lua_pushnumber(L, end);

    return 2;
}

int Wrap_ParticleSystem::setSpinVariation(lua_State* L)
{
    auto* self      = luax_checkparticlesystem(L, 1);
    float variation = luaL_checknumber(L, 2);

    self->setSpinVariation(variation);

    return 0;
}

int Wrap_ParticleSystem::getSpinVariation(lua_State* L)
{
    auto* self      = luax_checkparticlesystem(L, 1);
    float variation = self->getSpinVariation();

    lua_pushnumber(L, variation);

    return 1;
}

int Wrap_ParticleSystem::setOffset(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);
    float x    = luaL_checknumber(L, 2);
    float y    = luaL_checknumber(L, 3);

    self->setOffset(x, y);

    return 0;
}

int Wrap_ParticleSystem::getOffset(lua_State* L)
{
    auto* self     = luax_checkparticlesystem(L, 1);
    Vector2 offset = self->getOffset();

    lua_pushnumber(L, offset.x);
    lua_pushnumber(L, offset.y);

    return 2;
}

int Wrap_ParticleSystem::setColors(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);

    if (lua_istable(L, 2))
    {
        int count = lua_gettop(L) - 1;

        if (count > 8)
            return luaL_error(L, "At most eight (8) colors may be used.");

        std::vector<Color> colors(count);
        for (int index = 0; index < count; index++)
        {
            luaL_checktype(L, index + 2, LUA_TTABLE);

            if (lua_objlen(L, index + 2) < 3)
                return luaL_argerror(L, index + 2, "expected 4 color components");

            for (int j = 0; j < 4; j++)
                lua_rawgeti(L, index + 2, j + 1);

            colors[index].r = luaL_checknumber(L, -4);
            colors[index].g = luaL_checknumber(L, -3);
            colors[index].b = luaL_checknumber(L, -2);
            colors[index].a = luaL_optnumber(L, -1, 1.0f);

            lua_pop(L, 4);
        }

        self->setColor(colors);
    }
    else
    {
        int argc  = lua_gettop(L) - 1;
        int count = (argc + 3) / 4;

        if (argc != 3 && (argc % 4 != 0 || argc == 0))
            return luaL_error(L, "Expected 3 components, got %d.", argc % 4);

        if (count > 8)
            return luaL_error(L, "At most eight (8) colors may be used.");

        std::vector<Color> colors(count);

        for (int index = 0; index < count; ++index)
        {
            colors[index].r = luaL_checknumber(L, 1 + index * 4 + 1);
            colors[index].g = luaL_checknumber(L, 1 + index * 4 + 2);
            colors[index].b = luaL_checknumber(L, 1 + index * 4 + 3);
            colors[index].a = luaL_checknumber(L, 1 + index * 4 + 4);
        }

        self->setColor(colors);
    }

    return 0;
}

int Wrap_ParticleSystem::getColors(lua_State* L)
{
    auto* self         = luax_checkparticlesystem(L, 1);
    const auto& colors = self->getColor();

    for (size_t index = 0; index < colors.size(); index++)
    {
        lua_createtable(L, 4, 0);

        lua_pushnumber(L, colors[index].r);
        lua_rawseti(L, -2, 1);
        lua_pushnumber(L, colors[index].g);
        lua_rawseti(L, -2, 2);
        lua_pushnumber(L, colors[index].b);
        lua_rawseti(L, -2, 3);
        lua_pushnumber(L, colors[index].a);
        lua_rawseti(L, -2, 4);
    }

    return (int)colors.size();
}

int Wrap_ParticleSystem::setQuads(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);
    std::vector<Quad*> quads;

    if (lua_istable(L, 2))
    {
        for (int index = 1; index < (int)luax_objlen(L, 2); index++)
        {
            lua_rawgeti(L, 2, index);

            auto* quad = luax_checktype<Quad>(L, -1);
            quads.push_back(quad);

            lua_pop(L, 1);
        }
    }
    else
    {
        for (int index = 2; index <= lua_gettop(L); index++)
        {
            auto* quad = luax_checktype<Quad>(L, index);
            quads.push_back(quad);
        }
    }

    self->setQuads(quads);

    return 0;
}

int Wrap_ParticleSystem::getQuads(lua_State* L)
{
    auto* self       = luax_checkparticlesystem(L, 1);
    const auto quads = self->getQuads();

    lua_createtable(L, (int)quads.size(), 0);

    for (int index = 0; index < (int)quads.size(); index++)
    {
        luax_pushtype(L, quads[index]);
        lua_rawseti(L, -2, index + 1);
    }

    return 1;
}

int Wrap_ParticleSystem::setRelativeRotation(lua_State* L)
{
    auto* self  = luax_checkparticlesystem(L, 1);
    bool enable = luax_checkboolean(L, 2);

    self->setRelativeRotation(enable);

    return 0;
}

int Wrap_ParticleSystem::hasRelativeRotation(lua_State* L)
{
    auto* self   = luax_checkparticlesystem(L, 1);
    bool enabled = self->hasRelativeRoation();

    luax_pushboolean(L, enabled);

    return 1;
}

int Wrap_ParticleSystem::getCount(lua_State* L)
{
    auto* self     = luax_checkparticlesystem(L, 1);
    uint32_t count = self->getCount();

    lua_pushnumber(L, count);

    return 1;
}

int Wrap_ParticleSystem::start(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);
    self->start();

    return 0;
}

int Wrap_ParticleSystem::stop(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);
    self->stop();

    return 0;
}

int Wrap_ParticleSystem::pause(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);
    self->pause();

    return 0;
}

int Wrap_ParticleSystem::reset(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);
    self->reset();

    return 0;
}

int Wrap_ParticleSystem::emit(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);
    int count  = luaL_checkinteger(L, 2);

    self->emit(count);

    return 0;
}

int Wrap_ParticleSystem::isActive(lua_State* L)
{
    auto* self  = luax_checkparticlesystem(L, 1);
    bool active = self->isActive();

    luax_pushboolean(L, active);

    return 1;
}

int Wrap_ParticleSystem::isPaused(lua_State* L)
{
    auto* self  = luax_checkparticlesystem(L, 1);
    bool paused = self->isPaused();

    luax_pushboolean(L, paused);

    return 1;
}

int Wrap_ParticleSystem::isStopped(lua_State* L)
{
    auto* self   = luax_checkparticlesystem(L, 1);
    bool stopped = self->isStopped();

    luax_pushboolean(L, stopped);

    return 1;
}

int Wrap_ParticleSystem::update(lua_State* L)
{
    auto* self = luax_checkparticlesystem(L, 1);
    float dt   = luaL_checknumber(L, 2);

    self->update(dt);

    return 0;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clone",                     Wrap_ParticleSystem::clone                     },
    { "setTexture",                Wrap_ParticleSystem::setTexture                },
    { "getTexture",                Wrap_ParticleSystem::getTexture                },
    { "setBufferSize",             Wrap_ParticleSystem::setBufferSize             },
    { "getBufferSize",             Wrap_ParticleSystem::getBufferSize             },
    { "setInsertMode",             Wrap_ParticleSystem::setInsertMode             },
    { "getInsertMode",             Wrap_ParticleSystem::getInsertMode             },
    { "setEmissionRate",           Wrap_ParticleSystem::setEmissionRate           },
    { "getEmissionRate",           Wrap_ParticleSystem::getEmissionRate           },
    { "setEmitterLifetime",        Wrap_ParticleSystem::setEmitterLifetime        },
    { "getEmitterLifetime",        Wrap_ParticleSystem::getEmitterLifetime        },
    { "setParticleLifetime",       Wrap_ParticleSystem::setParticleLifetime       },
    { "getParticleLifetime",       Wrap_ParticleSystem::getParticleLifetime       },
    { "setPosition",               Wrap_ParticleSystem::setPosition               },
    { "getPosition",               Wrap_ParticleSystem::getPosition               },
    { "moveTo",                    Wrap_ParticleSystem::moveTo                    },
    { "setEmissionArea",           Wrap_ParticleSystem::setEmissionArea           },
    { "getEmissionArea",           Wrap_ParticleSystem::getEmissionArea           },
    { "setDirection",              Wrap_ParticleSystem::setDirection              },
    { "getDirection",              Wrap_ParticleSystem::getDirection              },
    { "setSpread",                 Wrap_ParticleSystem::setSpread                 },
    { "getSpread",                 Wrap_ParticleSystem::getSpread                 },
    { "setSpeed",                  Wrap_ParticleSystem::setSpeed                  },
    { "getSpeed",                  Wrap_ParticleSystem::getSpeed                  },
    { "setLinearAcceleration",     Wrap_ParticleSystem::setLinearAcceleration     },
    { "getLinearAcceleration",     Wrap_ParticleSystem::getLinearAcceleration     },
    { "setRadialAcceleration",     Wrap_ParticleSystem::setRadialAcceleration     },
    { "getRadialAcceleration",     Wrap_ParticleSystem::getRadialAcceleration     },
    { "setTangentialAcceleration", Wrap_ParticleSystem::setTangentialAcceleration },
    { "getTangentialAcceleration", Wrap_ParticleSystem::getTangentialAcceleration },
    { "setLinearDamping",          Wrap_ParticleSystem::setLinearDamping          },
    { "getLinearDamping",          Wrap_ParticleSystem::getLinearDamping          },
    { "setSizes",                  Wrap_ParticleSystem::setSizes                  },
    { "getSizes",                  Wrap_ParticleSystem::getSizes                  },
    { "setSizeVariation",          Wrap_ParticleSystem::setSizeVariation          },
    { "getSizeVariation",          Wrap_ParticleSystem::getSizeVariation          },
    { "setRotation",               Wrap_ParticleSystem::setRotation               },
    { "getRotation",               Wrap_ParticleSystem::getRotation               },
    { "setSpin",                   Wrap_ParticleSystem::setSpin                   },
    { "getSpin",                   Wrap_ParticleSystem::getSpin                   },
    { "setSpinVariation",          Wrap_ParticleSystem::setSpinVariation          },
    { "getSpinVariation",          Wrap_ParticleSystem::getSpinVariation          },
    { "setOffset",                 Wrap_ParticleSystem::setOffset                 },
    { "getOffset",                 Wrap_ParticleSystem::getOffset                 },
    { "setColors",                 Wrap_ParticleSystem::setColors                 },
    { "getColors",                 Wrap_ParticleSystem::getColors                 },
    { "setQuads",                  Wrap_ParticleSystem::setQuads                  },
    { "getQuads",                  Wrap_ParticleSystem::getQuads                  },
    { "setRelativeRotation",       Wrap_ParticleSystem::setRelativeRotation       },
    { "hasRelativeRotation",       Wrap_ParticleSystem::hasRelativeRotation       },
    { "getCount",                  Wrap_ParticleSystem::getCount                  },
    { "start",                     Wrap_ParticleSystem::start                     },
    { "stop",                      Wrap_ParticleSystem::stop                      },
    { "pause",                     Wrap_ParticleSystem::pause                     },
    { "reset",                     Wrap_ParticleSystem::reset                     },
    { "emit",                      Wrap_ParticleSystem::emit                      },
    { "isActive",                  Wrap_ParticleSystem::isActive                  },
    { "isPaused",                  Wrap_ParticleSystem::isPaused                  },
    { "isStopped",                 Wrap_ParticleSystem::isStopped                 },
    { "update",                    Wrap_ParticleSystem::update                    }
};
// clang-format on

namespace love
{
    ParticleSystem* luax_checkparticlesystem(lua_State* L, int index)
    {
        return luax_checktype<ParticleSystem>(L, index);
    }

    int open_particlesystem(lua_State* L)
    {
        return luax_register_type(L, &ParticleSystem::type, functions);
    }
} // namespace love
