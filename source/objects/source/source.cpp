#include "common/runtime.h"

#include "objects/source/source.h"
#include "modules/audio.h"

Source::Source(const char * path, bool stream)
{
	if (stream)
		this->music = Mix_LoadMUS(path);
	else
		this->sound = Mix_LoadWAV(path);

	this->channel = Audio::GetOpenChannel();
	this->stream = stream;
}

bool Source::IsPlaying()
{
	if (!this->stream)
		return Mix_Playing(this->channel);
	else
		return Mix_PlayingMusic();
}

void Source::Pause()
{
	if (!this->stream)
		Mix_Pause(this->channel);
	else
		Mix_PauseMusic();
}

void Source::Stop()
{
	if (!this->stream)
		Mix_HaltChannel(this->channel);
	else
		Mix_HaltMusic();
}

void Source::Resume()
{
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
	int loops = (this->loop) ? -1 : 0;

	if (!this->stream)
		Mix_PlayChannel(this->channel, this->sound, loops);
	else
		Mix_PlayMusic(this->music, loops);
}