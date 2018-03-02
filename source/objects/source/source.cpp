#include "common/runtime.h"
#include "objects/source/source.h"

Source::Source(const char * path, bool stream)
{
	this->fileHandle = fopen(path, "rb");
	if (!this->fileHandle)
	{
		fclose(this->fileHandle);
		Console::ThrowError("Could not open %s: does not exist.", path);
	}

	this->Decode();
}

void Source::Decode()
{
	char buff[8];
	bool valid = true;

	fread(buff, 4, 1, this->fileHandle); // chunkId
	if (strncmp(buff, "RIFF", 4) != 0)
		valid = false;

	fseek(this->fileHandle, 4, SEEK_CUR); // skip ckSize

	fread(buff, 4, 1, this->fileHandle); // WAVEID
	if (strncmp(buff, "WAVE", 4) != 0)
		valid = false;

	fread(buff, 4, 1, this->fileHandle); // fmt
	if (strncmp(buff, "fmt", 3) != 0)
		valid = false;
	
	fread(buff, 4, 1, this->fileHandle); // format type
	if (*buff != 16)
		valid = false; // needs to be PCM 16

	fread(buff, 2, 1, this->fileHandle); // wFormatTag
	if (*buff != 0x0001)
		valid = false;
	
	fread(&this->channels, 2, 1, this->fileHandle); // channel count

	fread(&this->rate, 4, 1, this->fileHandle); // nSamplesPerSec

	fseek(this->fileHandle, 4, SEEK_CUR); // skip nAvgBytesPerSec

	u16 byte_per_block; // 1 block = 1*channelCount samples
	fread(&byte_per_block, 2, 1, this->fileHandle); // nBlockAlign

	u16 byte_per_sample;
	fread(&byte_per_sample, 2, 1, this->fileHandle); // wBitsPerSample
	byte_per_sample /= 8; // bits -> bytes

	fread(&buff, 4, 1, this->fileHandle); // ckId

	while (strncmp(buff, "data", 4) != 0) 
	{
		u32 ckSize;
		fread(&ckSize, 4, 1, this->fileHandle); // ckSize

		fseek(this->fileHandle, ckSize, SEEK_CUR); // skip chunk

		int i = fread(&buff, 4, 1, this->fileHandle); // next chunk ckId

		if (i < 4) { // reached EOF before finding a data chunk
			valid = false;
			break;
		}
	}

	fread(&this->size, 4, 1, this->fileHandle);

	this->nsamples = this->size / byte_per_block;

	if (byte_per_sample != 2) // is not PCM 16
		valid = false;

	if (!valid)
	{
		fclose(this->fileHandle);
		printf("Invalid wave file\n");
		return;
	}

	u32 raw_data_size_aligned = (this->size + 0xfff) & ~0xfff;
	this->data = (u8 *)memalign(0x1000, raw_data_size_aligned);
	memset(this->data, 0, raw_data_size_aligned);

	fread(this->data, this->size, 1, this->fileHandle);
}

void Source::Play()
{
	this->buffer.next = 0;
	this->buffer.buffer = this->data;
	this->buffer.buffer_size = this->rate;
	this->buffer.data_size = this->size;
	this->buffer.data_offset = 0;

	audoutAppendAudioOutBuffer(&this->buffer);
}