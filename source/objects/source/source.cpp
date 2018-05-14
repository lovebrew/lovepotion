#include "runtime.h"

#include "source.h"
#include "audio.h"

Source::Source(const char * path, bool stream)
{
	for (int i = 0; i < 12; i++) 
		this->mix[i] = 1.0f;
		
	this->interp = NDSP_INTERP_LINEAR;

	this->stream = stream;
	this->loop = false;
	this->reset = false;
	this->fillBuffer = false;

	this->fileHandle = fopen(path, "r");

	this->path = strdup(path);

	if (!this->fileHandle)
		Console::ThrowError("%s: %s", strerror(errno), path);
	else
		this->Decode();
}

void Source::Decode()
{
	if (ov_open(this->fileHandle, &this->vorbisFile, NULL, 0) < 0)
		Console::ThrowError("%s", "Invalid ogg vorbis file");

	vorbis_info * vorbisInfo = ov_info(&this->vorbisFile, -1);

	if (!vorbisInfo) //No ogg info
		printf("Could not retrieve vorbis information\n");

	this->rate = (float)vorbisInfo->rate;

	this->channels = (u32)vorbisInfo->channels;

	this->encoding = NDSP_ENCODING_PCM16;

	this->nsamples = (u32)ov_pcm_total(&this->vorbisFile, -1);

	this->bitrate = (u32)ov_bitrate(&this->vorbisFile, -1);

	this->audiochannel = Audio::GetOpenChannel();
	printf("Channel: %d; Path: %s\n", this->audiochannel, this->path);

	memset(this->waveBuffer, 0, sizeof(this->waveBuffer));

	if (!this->stream)
	{
		this->size = this->nsamples * this->channels * 2; // *2 because output is PCM16 (2 bytes/sample)
		
		if (linearSpaceFree() < this->size)
		{
			printf("%s", "Not enough linear memory");
			Console::ThrowError("%s", "Not enough linear memory");
		}

		this->data = (char *)linearAlloc(this->size);
		
		this->waveBuffer[0].data_vaddr = this->data;

		this->FillBuffer((char *)this->waveBuffer[0].data_vaddr);

		ov_clear(&this->vorbisFile);

		fclose(this->fileHandle);
	}
	else
	{
		this->size = 4096;
		int chunkSamples = this->size / this->channels / 2;

		if (linearSpaceFree() < this->size * 2)
			Console::ThrowError("%s", "Not enough linear memory");

		this->waveBuffer[0].data_vaddr = linearAlloc(this->size);
		this->waveBuffer[0].nsamples = chunkSamples;
		
		this->waveBuffer[1].data_vaddr = linearAlloc(this->size);
		this->waveBuffer[1].nsamples = chunkSamples;
		this->waveBuffer[1].status = NDSP_WBUF_DONE;

		this->FillBuffer((char *)this->waveBuffer[0].data_vaddr);
	}
}

void Source::Play()
{
	//printf("Playing %s\n", this->path);
	if (this->audiochannel == -1) 
	{
		Console::ThrowError("%s", "Playback failure: no audio channel.");
		return;
	}

	ndspChnWaveBufClear(this->audiochannel);
	ndspChnReset(this->audiochannel);
	
	ndspChnInitParams(this->audiochannel);
	ndspChnSetMix(this->audiochannel, this->mix);
	ndspChnSetInterp(this->audiochannel, this->interp);
	ndspChnSetRate(this->audiochannel, this->rate);
	ndspChnSetFormat(this->audiochannel, NDSP_CHANNELS(this->channels) | NDSP_ENCODING(this->encoding));

	if (this->stream)
	{	
		ndspChnWaveBufAdd(this->audiochannel, &this->waveBuffer[0]);
		this->fillBuffer = !this->fillBuffer;
	} 
	else 
	{
		this->waveBuffer[0].nsamples = this->nsamples;
		this->waveBuffer[0].looping = this->loop;

		ndspChnWaveBufAdd(this->audiochannel, &this->waveBuffer[0]);
		DSP_FlushDataCache((u32 *)this->data, this->size);
	}

	this->playing = true;
	this->reset = false;
}

void Source::Stop()
{
	this->playing = false;
	ndspChnWaveBufClear(this->audiochannel);
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
	if (this->stream)
		return this->playing;
	else
		return ndspChnIsPlaying(this->audiochannel);
}

void Source::SetVolume(float volume)
{
	volume = std::min(std::max(volume, 0.0f), 1.0f);

	for (int i = 0; i<= 3; i++) 
		this->mix[i] = volume;

	ndspChnSetMix(this->audiochannel, this->mix);
}

void Source::Update()
{
	if (this->waveBuffer[this->fillBuffer].status == NDSP_WBUF_DONE)
	{
		this->FillBuffer((char *)this->waveBuffer[this->fillBuffer].data_vaddr);

		if (this->reset && !this->loop)
			return;

		ndspChnWaveBufAdd(this->audiochannel, &this->waveBuffer[this->fillBuffer]);

		this->fillBuffer = !this->fillBuffer;
	}
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
		long ret = ov_read(&this->vorbisFile, &destination[offset], fmin(this->size - offset, 4096), &this->currentSection);

		if (ret == 0)
		{
			if (this->stream && feof(this->fileHandle))
				this->Reset();
			
			eof = 1;
		} 
		else if (ret < 0) 
		{
			ov_clear(&this->vorbisFile);

			linearFree(destination);
 
			break;
		}
		else 
		{
			offset += ret;
		}
	}

	if (this->stream && !feof(this->fileHandle))
		DSP_FlushDataCache((u32 *)audio, this->size);
}

int Source::GetAudioChannel()
{
	return this->audiochannel;
}

const char * Source::GetPath()
{
	return this->path;
}

Source::~Source()
{
	ndspChnWaveBufClear(this->audiochannel);
	
	if (this->data != nullptr)
		linearFree(this->data);
	
	if (this->stream)
	{
		linearFree((void *)this->waveBuffer[0].data_vaddr);
		linearFree((void *)this->waveBuffer[1].data_vaddr);
	}

	audioChannels[this->audiochannel] = false;
}