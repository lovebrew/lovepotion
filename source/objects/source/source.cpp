#include "common/runtime.h"

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include "objects/source/source.h"

Source::Source(const char * path, bool stream)
{
	this->fileHandle = fopen(path, "rb");
	if (!this->fileHandle)
	{
		fclose(this->fileHandle);
		throw Exception("Could not open %s: does not exist.", path);
	}

	this->Decode();
}

void Source::Decode()
{
	if (ov_open(this->fileHandle, &this->vorbisFile, NULL, 0) < 0)
		throw Exception("Invalid ogg vorbis file");

	vorbis_info * vorbisInfo = ov_info(&this->vorbisFile, -1);

	if (vorbisInfo == NULL) //No ogg info
		throw Exception("Could not retrieve vorbis information");

	this->rate = (float)vorbisInfo->rate;

	this->channels = (u32)vorbisInfo->channels;

	//this->encoding = NDSP_ENCODING_PCM16;

	this->nsamples = (u32)ov_pcm_total(&this->vorbisFile, -1);

	this->bitrate = (u32)ov_bitrate(&this->vorbisFile, -1);

	this->loop = false;

	this->size = this->nsamples * this->channels * 2; // *2 because output is PCM16 (2 bytes/sample)

	this->rawSize = (this->size + 0xfff) & ~0xfff;
	this->data = (char *)memalign(0x1000, this->rawSize);
	memset(this->data, 0, this->rawSize);

	this->FillBuffer((char *)this->data);

	ov_clear(&this->vorbisFile);

	fclose(this->fileHandle);
}

void Source::Reset()
{
	fseek(this->fileHandle, 0, SEEK_SET);
	ov_pcm_seek(&this->vorbisFile, 0);
	this->currentSection = 0;
	this->reset = true;
}

void Source::FillBuffer(void * audio)
{
	int eof = 0;
	int offset = 0;
	
	char * destination = (char *)audio;

	while (!eof)
	{
		long ret = ov_read(&this->vorbisFile, &destination[offset], fmin(this->size - offset, 4096), 0, 2, 1, &this->currentSection);

		if (ret == 0)
		{
			if (this->stream && feof(this->fileHandle))
				this->Reset();
			
			eof = 1;
		} 
		else if (ret < 0) 
		{
			ov_clear(&this->vorbisFile);

			free(destination);
 
			break;
		}
		else
			offset += ret;
	}

	//if (this->stream && !feof(this->fileHandle))
		//DSP_FlushDataCache((u32 *)audio, this->size);
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