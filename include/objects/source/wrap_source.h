#pragma once

int sourceNew(lua_State * L);

int sourcePlay(lua_State * L);

int sourceStop(lua_State * L);

int sourceSetLooping(lua_State * L);

int sourceSetVolume(lua_State * L);

int sourceIsPlaying(lua_State * L);

int sourceGetDuration(lua_State * L);

int sourceTell(lua_State * L);

void sourceStream(void * arg);

int sourceGC(lua_State * L);

int initSourceClass(lua_State * L);

extern volatile bool updateAudioThread;