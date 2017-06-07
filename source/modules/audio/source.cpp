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

	love_source_type t = TYPE_WAV;
	if (strncmp((extension + 1), "ogg", 3) == 0)
		t = TYPE_OGG;

	this->stream = (strncmp(type, "stream", 6) == 0) ? true : false;

	this->file = fopen(path, "r");

	if (t == TYPE_WAV)
		this->DecodeWave();

	return nullptr;
}

void Source::DecodeWave()
{
	bool valid = true;

	char buff[8];

	// Master chunk
	fread(buff, 4, 1, file); // ckId
	if (strncmp(buff, "RIFF", 4) != 0) valid = false;

	fseek(file, 4, SEEK_CUR); // skip ckSize

	fread(buff, 4, 1, file); // WAVEID
	if (strncmp(buff, "WAVE", 4) != 0) valid = false;

	// fmt Chunk
	fread(buff, 4, 1, file); // ckId
	if (strncmp(buff, "fmt ", 4) != 0) valid = false;

	fread(buff, 4, 1, file); // ckSize
	if (*buff != 16) valid = false; // should be 16 for PCM format

	fread(buff, 2, 1, file); // wFormatTag
	if (*buff != 0x0001) valid = false; // PCM format

	u16 channels;
	fread(&channels, 2, 1, file); // nChannels
	this->channels = channels;
				
	u32 rate;
	fread(&rate, 4, 1, file); // nSamplesPerSec
	this->rate = rate;

	fseek(file, 4, SEEK_CUR); // skip nAvgBytesPerSec

	u16 byte_per_block; // 1 block = 1*channelCount samples
	fread(&byte_per_block, 2, 1, file); // nBlockAlign

	u16 byte_per_sample;
	fread(&byte_per_sample, 2, 1, file); // wBitsPerSample
	byte_per_sample /= 8; // bits -> bytes

	// There may be some additionals chunks between fmt and data
	fread(&buff, 4, 1, file); // ckId
	while (strncmp(buff, "data", 4) != 0) 
	{
		u32 size;
		fread(&size, 4, 1, file); // ckSize

		fseek(file, size, SEEK_CUR); // skip chunk

		int i = fread(&buff, 4, 1, file); // next chunk ckId

		if (i < 4) // reached EOF before finding a data chunk
		{
			valid = false;
			break;
		}
	}

	// data Chunk (ckId already read)
	u32 size;
	fread(&size, 4, 1, file); // ckSize
	this->size = size;

	this->nsamples = this->size / byte_per_block;

	if (byte_per_sample == 1) 
		this->encoding = NDSP_ENCODING_PCM8;
	else if (byte_per_sample == 2) 
		this->encoding = NDSP_ENCODING_PCM16;
	else 
		printf("unknown encoding, needs to be PCM8 or PCM16\n");

	if (!valid) 
	{
		fclose(file);
		printf("invalid PCM wav file");
	}

	this->audiochannel = this->GetOpenChannel();
	this->loop = false;

	if (!this->stream)
	{
		if (linearSpaceFree() < this->size) 
			printf("not enough linear memory available");
		
		this->data = (char *)linearAlloc(this->size);

		fread(this->data, this->size, 1, file);
	}
	else
	{

		this->samplesPerBuffer = this->rate / 30;
		//printf("%d %d\n", this->nsamples, this->nsamples / 60);
		this->data = (char *)linearAlloc(this->samplesPerBuffer * byte_per_sample * 2);

		memset(this->waveBuffer, 0, sizeof(this->waveBuffer));

		this->waveBuffer[0].data_vaddr = &this->data[0];
		this->waveBuffer[0].nsamples = this->samplesPerBuffer;

		this->waveBuffer[1].data_vaddr = &this->data[this->samplesPerBuffer];
		this->waveBuffer[1].nsamples = this->samplesPerBuffer;

		this->FillBuffer(this->data, this->streamOffset, this->samplesPerBuffer * 2);

		this->streamOffset += this->samplesPerBuffer;

		streams[this->audiochannel] = this;
	}
}

char * Source::GetAudio()
{
	return this->data;
}

u32 Source::GetSamples()
{
	return this->nsamples;
}

int Source::GetOpenChannel() {

	for (int i = 0; i <= 23; i++) {
		if (!channelList[i]) {
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
		this->FillBuffer(this->waveBuffer[this->fillBuffer].data_pcm16, this->streamOffset, this->waveBuffer[this->fillBuffer].nsamples);
		ndspChnWaveBufAdd(this->audiochannel, &this->waveBuffer[this->fillBuffer]);
		
		this->streamOffset += this->waveBuffer[this->fillBuffer].nsamples;

		this->fillBuffer = !this->fillBuffer;
	}
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
		ndspWaveBuf* waveBuf = (ndspWaveBuf *)calloc(1, sizeof(ndspWaveBuf));

		waveBuf->data_vaddr = this->data;
		waveBuf->nsamples = this->nsamples;
		waveBuf->looping = this->loop;

		DSP_FlushDataCache((u32*)this->data, this->size);

		ndspChnWaveBufAdd(this->audiochannel, waveBuf);
	}
	
}

void Source::FillBuffer(void * audio, size_t offset, size_t ssize)
{
	u32 * destination = (u32 *)audio;

	//for (int i = 0; i < size; i++)
	fread(destination, ssize, 1, this->file);

	DSP_FlushDataCache(audio, ssize);
}