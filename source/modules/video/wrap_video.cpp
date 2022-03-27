#include "modules/video/wrap_video.h"
#include "objects/video/wrap_videostream.h"

#include "modules/filesystem/wrap_filesystem.h"
#include "objects/file/file.h"
#include "objects/video/videostream.h"

using namespace love;

#define instance() (Module::GetInstance<Video>(Module::M_VIDEO))

int Wrap_Video::NewVideoStream(lua_State* L)
{
    File* file = Wrap_Filesystem::GetFile(L, 1);

    VideoStream* stream = nullptr;
    Luax::CatchException(L, [&]() {
        if (!file->IsOpen() && !file->Open(File::MODE_READ))
            luaL_error(L, "File is not open and canot be opened.");

        stream = instance()->NewVideoStream(file);
    });

    Luax::PushType(L, stream);

    stream->Release();
    file->Release();

    return 1;
}

// clang-format off
static constexpr lua_CFunction types[] =
{
    Wrap_VideoStream::Register,
    0
};

static constexpr luaL_Reg functions[] =
{
    { "newVideoStream", Wrap_Video::NewVideoStream },
    { 0               , 0                          }
};
// clang-format on

int Wrap_Video::Register(lua_State* L)
{
    Video* instance = instance();
    if (instance == nullptr)
        Luax::CatchException(L, [&]() { instance = new Video(); });
    else
        instance->Retain();

    WrappedModule module;
    module.instance  = instance;
    module.name      = "video";
    module.type      = &Module::type;
    module.functions = functions;
    module.types     = types;

    return Luax::RegisterModule(L, module);
}
