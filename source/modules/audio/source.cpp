#include "common/runtime.h"
#include "source.h"

using love::Source;

bool channelList[24];

const char * Source::Init(const char * path, const char * type)
{
	for (int i = 0; i < 12; i++) 
	{
		this->mix[i] = 1.0f;
		this->interp = NDSP_INTERP_LINEAR;
	}

	const char * extension = strrchr(path, '.');

	if (strncmp((extension + 1), "ogg", 3) != 0)
		return "Invalid Source type!";

	this->stream = (strncmp(type, "stream", 6) == 0) ? true : false;

	this->fileHandle = fopen(path, "r");
	
	const char * error; 
	if (!this->fileHandle)
		return "Failed to open file.";

	this->fillBuffer = false;
	this->firstFill = true;
	this->playing = false;

	error = this->Decode();

	if (error != nullptr)
		return error;

	return nullptr;
}

const char * Source::Decode()
{	
	if (ov_open(this->fileHandle, &this->vorbisFile, NULL, 0) < 0)
		return "Invalid ogg vorbis file";

	vorbis_info * vorbisInfo = ov_info(&this->vorbisFile, -1);

	if (vorbisInfo == NULL) //No ogg info
		return "Could not retrieve vorbis information";

	this->rate = (float)vorbisInfo->rate;

	this->channels = (u32)vorbisInfo->channels;

	this->encoding = NDSP_ENCODING_PCM16;

	this->nsamples = (u32)ov_pcm_total(&this->vorbisFile, -1);

	this->bitrate = (u32)ov_bitrate(&this->vorbisFile, -1);

	this->audiochannel = this->GetOpenChannel();

	this->loop = false;

	if (!this->stream)
	{
		this->size = this->nsamples * this->channels * 2; // *2 because output is PCM16 (2 bytes/sample)
		
		if (linearSpaceFree() < this->size)
			return "not enough linear memory available";

		memset(this->waveBuffer, 0, sizeof(this->waveBuffer));

		this->data = (char *)linearAlloc(this->size);
		
		this->waveBuffer[0].data_vaddr = this->data;

		this->FillBuffer((char *)this->waveBuffer[0].data_vaddr);

		ov_clear(&this->vorbisFile);

		fclose(this->fileHandle);
	}
	else
	{
		this->size = 4096;
		this->chunkSamples = this->size / this->channels / 2;

		if (linearSpaceFree() < this->size * 2)
			return "not enough linear memory available";

		memset(this->waveBuffer, 0, sizeof(this->waveBuffer));

		this->waveBuffer[0].data_vaddr = linearAlloc(this->size);
		this->waveBuffer[0].nsamples = this->chunkSamples;
		
		this->waveBuffer[1].data_vaddr = linearAlloc(this->size);
		this->waveBuffer[1].nsamples = this->chunkSamples;
		this->waveBuffer[1].status = NDSP_WBUF_DONE;

		this->FillBuffer((char *)this->waveBuffer[0].data_vaddr);

		streams.push_back(this);
	}

	return nullptr;
}

int Source::GetOpenChannel() {
	for (int i = 0; i <= 23; i++)
	{
		if (!channelList[i]) 
		{
			channelList[i] = true;
			return i;
		}
	}

	return -1;
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

void Source::Play()
{
	if (this->audiochannel == -1) 
		return;

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

void Source::SetLooping(bool loop)
{
	this->loop = loop;
}

void Source::Stop()
{
	this->playing = false;
	ndspChnWaveBufClear(this->audiochannel);
}

int Source::IsPlaying()
{
	return this->playing;
}

void Source::SetVolume(float volume)
{
	volume = std::min(std::max(volume, 0.0f), 1.0f);

	for (int i = 0; i<= 3; i++) 
		this->mix[i] = volume;

	ndspChnSetMix(this->audiochannel, this->mix);
}

float Source::GetDuration()
{
	return (float)(this->nsamples) / this->rate;
}

float Source::Tell()
{
	if (!ndspChnIsPlaying(this->audiochannel))
		return 0;
	else
		return (float)(ndspChnGetSamplePos(this->audiochannel)) / this->rate;
}

long Source::FillBuffer(void * audio)
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

	return 0;
}

int Source::GetAudioChannel()
{
	return this->audiochannel;
}

bool Source::IsStatic()
{
	return (this->stream == false);
}

void Source::Collect()
{
	ndspChnWaveBufClear(this->audiochannel);
	
	if (this->data != nullptr)
		linearFree(this->data);
	
	if (this->stream)
	{
		linearFree((void *)this->waveBuffer[0].data_vaddr);
		linearFree((void *)this->waveBuffer[1].data_vaddr);
	}

	channelList[this->audiochannel] = false;
}