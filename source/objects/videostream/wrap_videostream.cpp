#include "objects/videostream/wrap_videostream.h"

#include "objects/source/source.h"
#include "objects/videostream/sync/sourcesync.h"

#include "objects/videostream/sync/deltasync.h"

using namespace love;

int Wrap_VideoStream::SetSync(lua_State* L)
{
    VideoStream* self = Wrap_VideoStream::CheckVideoStream(L, 1);

    if (Luax::IsType(L, 2, Source::type))
    {
        Source* source   = Luax::ToType<Source>(L, 2);
        SourceSync* sync = new SourceSync(source);

        self->SetSync(sync);
        sync->Release();
    }
    else if (Luax::IsType(L, 2, VideoStream::type))
    {
        VideoStream* other = Luax::ToType<VideoStream>(L, 2);
        self->SetSync(other->GetSync());
    }
    else if (lua_isnoneornil(L, 2))
    {
        DeltaSync* sync = new DeltaSync();
        sync->CopyState(self->GetSync());

        self->SetSync(sync);
        sync->Release();
    }
    else
        return Luax::TypeErrror(L, 2, "Source, VideoStream or nil");

    return 0;
}

int Wrap_VideoStream::GetFilename(lua_State* L)
{
    VideoStream* self = Wrap_VideoStream::CheckVideoStream(L, 1);

    Luax::PushString(L, self->GetFilename());

    return 1;
}

int Wrap_VideoStream::Play(lua_State* L)
{
    VideoStream* self = Wrap_VideoStream::CheckVideoStream(L, 1);

    self->Play();

    return 0;
}

int Wrap_VideoStream::Pause(lua_State* L)
{
    VideoStream* self = Wrap_VideoStream::CheckVideoStream(L, 1);

    self->Pause();

    return 0;
}

int Wrap_VideoStream::Seek(lua_State* L)
{
    VideoStream* self = Wrap_VideoStream::CheckVideoStream(L, 1);
    double offset     = luaL_checknumber(L, 2);

    self->Seek(offset);

    return 0;
}

int Wrap_VideoStream::Rewind(lua_State* L)
{
    VideoStream* self = Wrap_VideoStream::CheckVideoStream(L, 1);

    self->Seek(0);

    return 0;
}

int Wrap_VideoStream::Tell(lua_State* L)
{
    VideoStream* self = Wrap_VideoStream::CheckVideoStream(L, 1);

    lua_pushnumber(L, self->Tell());

    return 1;
}

int Wrap_VideoStream::IsPlaying(lua_State* L)
{
    VideoStream* self = Wrap_VideoStream::CheckVideoStream(L, 1);

    Luax::PushBoolean(L, self->IsPlaying());

    return 1;
}

VideoStream* Wrap_VideoStream::CheckVideoStream(lua_State* L, int index)
{
    return Luax::CheckType<VideoStream>(L, index);
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "setSync",     Wrap_VideoStream::SetSync     },
    { "getFilename", Wrap_VideoStream::GetFilename },
    { "play",        Wrap_VideoStream::Play        },
    { "pause",       Wrap_VideoStream::Pause       },
    { "seek",        Wrap_VideoStream::Seek        },
    { "rewind",      Wrap_VideoStream::Rewind      },
    { "tell",        Wrap_VideoStream::Tell        },
    { "isPlaying",   Wrap_VideoStream::IsPlaying   },
    { 0,             0                             }
};
// clang-format on

int Wrap_VideoStream::Register(lua_State* L)
{
    return Luax::RegisterType(L, &VideoStream::type, functions, nullptr);
}
