#include "runtime.h"

#include "source.h"
#include "audio.h"
#include "file.h"

Source::Source(const char * path, bool stream)
{
	File exists(path);

	if (!exists.Open("r"))
	{
		exists.Close();
		Console::ThrowError("File does not exist: %s", path);
	}

	if (stream)
		this->music = Mix_LoadMUS(path);
	else
		this->sound = Mix_LoadWAV(path);

	this->loop = false;

	if (!stream)
		this->channel = Audio::GetOpenChannel();
	
	this->stream = stream;
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

void Source::Stop()
{
	if (!this->IsValid())
		return;

	if (!this->stream)
		Mix_HaltChannel(this->channel);
	else
		Mix_HaltMusic();
}

void Source::SetLooping(bool loop)
{
	this->loop = loop;
}

bool Source::IsStatic()
{
	return (this->stream == false);
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

void Source::SetVolume(float volume)
{
	volume = std::min(std::max(volume, 0.0f), 1.0f);

	/*for (int i = 0; i<= 3; i++) 
		this->mix[i] = volume;

	ndspChnSetMix(this->audiochannel, this->mix);*/
}

bool Source::IsValid()
{
	if (this->stream && this->music)
		return true;
	else if (!this->stream && this->sound)
		return true;
	
	return false;
}

Source::~Source()
{
	if (!this->IsValid())
		return;

	if (!this->stream)
	{
		Mix_FreeChunk(this->sound);

		if (this->channel >= 0)
			audioChannels[this->channel] = false;
	}
	else
	{
		if (this->music)
			Mix_FreeMusic(this->music);
	}
}