#include "common/runtime.h"

#include "objects/source/source.h"
#include "modules/audio.h"

Source::Source(const char * path, const string & type) : Object("Source")
{
    if (type == "stream")
        this->music = Mix_LoadMUS(path);
    else
        this->sound = Mix_LoadWAV(path);

    this->loop = false;
    this->channel = Audio::GetOpenChannel();
    this->stream = (type == "stream");
}

Source::~Source()
{
    this->Stop();
}

bool Source::IsPlaying()
{
    if (!this->IsValid())
        return false;

    if (!this->stream)
        return Mix_Playing(this->channel);
    else
        return Mix_PlayingMusic();
}

void Source::Pause()
{
    if (!this->IsValid())
        return;

    if (!this->stream)
        Mix_Pause(this->channel);
    else
        Mix_PauseMusic();
}

void Source::Stop()
{
    if (!this->IsValid())
        return;

    if (!this->stream)
        Mix_HaltChannel(this->channel);
    else
        Mix_HaltMusic();
}

void Source::Resume()
{
    if (!this->IsValid())
        return;

    if (!this->stream)
        Mix_Resume(this->channel);
    else
        Mix_ResumeMusic();
}

bool Source::IsLooping()
{
    return this->loop;
}

void Source::SetLooping(bool loop)
{
    this->loop = loop;
}

void Source::Play()
{
    if (!this->IsValid())
        return;

    int loops = (this->loop) ? -1 : 0;

    if (!this->stream)
        Mix_PlayChannel(this->channel, this->sound, loops);
    else
        Mix_PlayMusic(this->music, loops);
}

bool Source::IsValid()
{
    if (this->stream && this->music)
        return true;
    else if (!this->stream && this->sound)
        return true;
    
    return false;
}