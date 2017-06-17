#include "common/runtime.h"
#include "source.h"

using love::Source;

bool channelList[24];

char * Source::Init(const char * path, const char * type)
{
	for (int i = 0; i < 12; i++) 
	{
		this->mix[i] = 1.0f;
		this->interp = NDSP_INTERP_LINEAR;
	}

	const char * extension = strrchr(path, '.');

	love_source_type t = TYPE_OGG;
	if (strncmp((extension + 1), "ogg", 3) != 0)
		return "Invalid Source type!";

	this->stream = (strncmp(type, "stream", 6) == 0) ? true : false;

	this->fileHandle = fopen(path, "r");
	
	char * error; 
	if (!this->fileHandle)
		error = "Failed to open file.";

	error = this->Decode();

	if (error != nullptr)
		return error;

	return nullptr;
}

char * Source::Decode()
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

	this->audiochannel = this->GetOpenChannel();

	this->loop = false;

	if (!this->stream)
	{
		this->size = this->nsamples * this->channels * 2; // *2 because output is PCM16 (2 bytes/sample)
		
		if (linearSpaceFree() < this->size)
			return "not enough linear memory available";

		this->data = (char *)linearAlloc(this->size);
		
		this->FillBuffer(this->data, true);

		ov_clear(&this->vorbisFile);

		fclose(this->fileHandle);
	}
	else
	{
		this->chunkSamples = round(this->rate * 0.1); //round(0.1 * this->rate);
		this->size = this->chunkSamples * this->channels * 2; // *2 because output is PCM16 (2 bytes/sample)

		if (linearSpaceFree() < this->size)
			return "not enough linear memory available";

		this->data = (char *)linearAlloc(this->size);

		long ret = this->FillBuffer(this->data, true);

		memset(this->waveBuffer, 0, sizeof(this->waveBuffer));

		this->waveBuffer[0].data_vaddr = &this->data[0];
		this->waveBuffer[0].nsamples = this->chunkSamples;

		this->waveBuffer[1].data_vaddr = &this->data[this->chunkSamples * 2];
		this->waveBuffer[1].nsamples = this->chunkSamples;

		streams[this->audiochannel] = this;
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
		this->FillBuffer((char *)this->waveBuffer[this->fillBuffer].data_vaddr, false);
		
		ndspChnWaveBufAdd(this->audiochannel, &this->waveBuffer[this->fillBuffer]);
		
		this->fillBuffer = !this->fillBuffer;
	}

	if (this->waveBuffer[0].status == NDSP_WBUF_DONE && this->waveBuffer[1].status == NDSP_WBUF_DONE)
	{
		if (feof(this->fileHandle) && this->loop)
		{
			this->Reset();
		}
	}	
}

void Source::Reset()
{

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
		ndspChnWaveBufAdd(this->audiochannel, &this->waveBuffer[1]);
	} 
	else 
	{
		this->waveBuffer[0].data_vaddr = this->data;
		this->waveBuffer[0].nsamples = this->nsamples;
		this->waveBuffer[0].looping = this->loop;

		ndspChnWaveBufAdd(this->audiochannel, &this->waveBuffer[0]);

		DSP_FlushDataCache(this->data, this->size);
	}
	
}
 
long Source::FillBuffer(void * audio, bool first)
{
	if (feof(this->fileHandle))
	{
		printf("eof!\n");
		return -1;
	}
	
	int eof = 0;
	int offset = 0;
	long ret = 0;

	int currentSection;
	
	char * destination = (char *)audio;

	while (!eof)
	{
		//int readSize = fmin(this->size - offset, 4096);
		//if (this->stream)
		//	readSize = this->chunkSamples * 2;

		ret = ov_read(&this->vorbisFile, &destination[offset], fmin(this->size - offset, 4096), &this->currentSection);

		if (ret == 0)
		{
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

			if (this->stream && offset >= this->size)
				break;
		}
	}

	if (this->stream)
		DSP_FlushDataCache((u32 *)audio, ret);

	return 0;
}