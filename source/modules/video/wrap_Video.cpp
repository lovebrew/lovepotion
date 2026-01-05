#include "modules/filesystem/wrap_Filesystem.hpp"

#include "modules/video/theora/Video.hpp"
#include "modules/video/wrap_Video.hpp"
#include "modules/video/wrap_VideoStream.hpp"

using namespace love;

#define instance() (Module::getInstance<VideoModuleBase>(Module::M_VIDEO))

int Wrap_Video::newVideoStream(lua_State* L)
{
    auto* file          = luax_getfile(L, 1);
    VideoStream* stream = nullptr;

    luax_catchexcept(L, [&]() {
        if (!file->isOpen() && !file->open(File::MODE_READ))
            luaL_error(L, "File is not open and cannot be opened.");

        stream = instance()->newVideoStream(file);
    });

    luax_pushtype(L, stream);
    stream->release();
    file->release();
    return 1;
}

static constexpr luaL_Reg functions[]  = { { "newVideoStream", Wrap_Video::newVideoStream } };
static constexpr lua_CFunction types[] = { love::open_videostream };

int Wrap_Video::open(lua_State* L)
{
    auto* instance = instance();

    if (instance == nullptr)
        luax_catchexcept(L, [&]() { instance = new VideoModule(); });
    else
        instance->retain();

    WrappedModule module {};
    module.instance  = instance;
    module.name      = "video";
    module.type      = &Module::type;
    module.functions = functions;
    module.types     = types;

    return luax_register_module(L, module);
}
