#include "common/luax.hpp"

#include "modules/love/love.hpp"

#include <3ds.h>

using namespace love;

static constexpr int SOC_BUFFER_SIZE  = 0x100000;
static constexpr int SOC_BUFFER_ALIGN = 0x1000;

static inline uint32_t* socBuffer = nullptr;

template<>
void love::PreInit<Console::CTR>()
{
    socBuffer = (uint32_t*)aligned_alloc(SOC_BUFFER_ALIGN, SOC_BUFFER_SIZE);
    socInit(socBuffer, SOC_BUFFER_SIZE);
}

template<>
bool love::MainLoop<Console::CTR>(lua_State* L, int numArgs)
{
    return (luax::Resume(L, numArgs) == LUA_YIELD && aptMainLoop());
}

template<>
void love::OnExit<Console::CTR>()
{
    socExit();
    free(socBuffer);
}
