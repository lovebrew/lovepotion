#include "common/runtime.h"

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include "objects/source/source.h"

Source::Source(const char * path, bool stream)
{
	printf("%d\n", GetAudioType(string(path)));
}

bool Source::IsPlaying()
{
	//AudioOutState state;
	//audoutGetAudioOutState(&state);

	return false;
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
	this->buffer.next = 0;
	this->buffer.buffer = this->data;
	this->buffer.buffer_size = this->rawSize;
	this->buffer.data_size = this->size;
	this->buffer.data_offset = 0;

	audoutAppendAudioOutBuffer(&this->buffer);
}