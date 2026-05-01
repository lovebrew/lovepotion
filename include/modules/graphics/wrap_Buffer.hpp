#include "common/luax.hpp"
#include "modules/graphics/Buffer.tcc"

namespace love
{
    void luax_writebufferdata(lua_State* L, int start, DataFormat format, char* data);

    void luax_readbufferdata(lua_State* L, DataFormat format, const char* data);

    BufferBase* luax_checkbuffer(lua_State* L, int index);

    int open_buffer(lua_State* L);
} // namespace love

namespace Wrap_Buffer
{
    int setArrayData(lua_State* L);

    int clear(lua_State* L);

    int getElementCount(lua_State* L);

    int getElementStride(lua_State* L);

    int getSize(lua_State* L);

    int getFormat(lua_State* L);

    int isBufferType(lua_State* L);

    int getDebugName(lua_State* L);
} // namespace Wrap_Buffer
