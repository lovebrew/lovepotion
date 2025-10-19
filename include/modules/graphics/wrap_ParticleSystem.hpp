#include "common/luax.hpp"
#include "modules/graphics/ParticleSystem.hpp"

namespace love
{
    ParticleSystem* luax_checkparticlesystem(lua_State* L, int index);

    int open_particlesystem(lua_State* L);
} // namespace love

namespace Wrap_ParticleSystem
{
    int clone(lua_State* L);

    int setTexture(lua_State* L);

    int getTexture(lua_State* L);

    int setBufferSize(lua_State* L);

    int getBufferSize(lua_State* L);

    int setInsertMode(lua_State* L);

    int getInsertMode(lua_State* L);

    int setEmissionRate(lua_State* L);

    int getEmissionRate(lua_State* L);

    int setEmitterLifetime(lua_State* L);

    int getEmitterLifetime(lua_State* L);

    int setParticleLifetime(lua_State* L);

    int getParticleLifetime(lua_State* L);

    int setPosition(lua_State* L);

    int getPosition(lua_State* L);

    int moveTo(lua_State* L);

    int setEmissionArea(lua_State* L);

    int getEmissionArea(lua_State* L);

    int setDirection(lua_State* L);

    int getDirection(lua_State* L);

    int setSpread(lua_State* L);

    int getSpread(lua_State* L);

    int setSpeed(lua_State* L);

    int getSpeed(lua_State* L);

    int setLinearAcceleration(lua_State* L);

    int getLinearAcceleration(lua_State* L);

    int setRadialAcceleration(lua_State* L);

    int getRadialAcceleration(lua_State* L);

    int setTangentialAcceleration(lua_State* L);

    int getTangentialAcceleration(lua_State* L);

    int setLinearDamping(lua_State* L);

    int getLinearDamping(lua_State* L);

    int setSizes(lua_State* L);

    int getSizes(lua_State* L);

    int setSizeVariation(lua_State* L);

    int getSizeVariation(lua_State* L);

    int setRotation(lua_State* L);

    int getRotation(lua_State* L);

    int setSpin(lua_State* L);

    int getSpin(lua_State* L);

    int setSpinVariation(lua_State* L);

    int getSpinVariation(lua_State* L);

    int setOffset(lua_State* L);

    int getOffset(lua_State* L);

    int setColors(lua_State* L);

    int getColors(lua_State* L);

    int setQuads(lua_State* L);

    int getQuads(lua_State* L);

    int setRelativeRotation(lua_State* L);

    int hasRelativeRotation(lua_State* L);

    int getCount(lua_State* L);

    int start(lua_State* L);

    int stop(lua_State* L);

    int pause(lua_State* L);

    int reset(lua_State* L);

    int emit(lua_State* L);

    int isActive(lua_State* L);

    int isPaused(lua_State* L);

    int isStopped(lua_State* L);

    int update(lua_State* L);
} // namespace Wrap_ParticleSystem
