#include "modplugdecoder.h"

#include "common/data.h"

using namespace love;

ModPlugDecoder::ModPlugDecoder(Stream* stream, int bufferSize) :
    Decoder(stream, bufferSize),
    plug(0),
    duration(-2.0)
{
    /* set some ModPlug settings. */
    this->settings.mFlags          = MODPLUG_ENABLE_OVERSAMPLING | MODPLUG_ENABLE_NOISE_REDUCTION;
    this->settings.mChannels       = 2;
    this->settings.mBits           = 16;
    this->settings.mFrequency      = sampleRate;
    this->settings.mResamplingMode = MODPLUG_RESAMPLE_LINEAR;

    /*
    ** fill with modplug defaults (modplug _memsets_, so we could get
    ** garbage settings when the struct is only partially initialized)
    ** This does not exist yet on Windows.
    */

    this->settings.mStereoSeparation = 128;
    this->settings.mMaxMixChannels   = 32;
    this->settings.mReverbDepth      = 0;
    this->settings.mReverbDelay      = 0;
    this->settings.mBassAmount       = 0;
    this->settings.mBassRange        = 0;
    this->settings.mSurroundDepth    = 0;
    this->settings.mSurroundDelay    = 0;
    this->settings.mLoopCount        = -1;

    ModPlug_SetSettings(&settings);

    // Load the module.
    if (stream->GetSize() > 1024 * 1024 * 4)
    {
        this->data.Set(stream->Read(1024 * 1024 * 4), Acquire::NORETAIN);
        this->plug = ModPlug_Load(this->data->GetData(), (int)this->data->GetSize());

        if (this->plug == nullptr)
            throw love::Exception("Could not load file with ModPlug.");

        this->stream->Seek(0);
        ModPlug_Unload(this->plug);
    }

    this->data.Set(this->stream->Read(this->stream->GetSize()), Acquire::NORETAIN);
    this->plug = ModPlug_Load(this->data->GetData(), (int)this->data->GetSize());

    if (this->plug == nullptr)
        throw love::Exception("Could not load file with ModPlug.");

    // set master volume for delicate ears
    ModPlug_SetMasterVolume(plug, 128);
}

ModPlugDecoder::~ModPlugDecoder()
{
    if (this->plug != nullptr)
        ModPlug_Unload(this->plug);
}

Decoder* ModPlugDecoder::Clone()
{
    StrongReference<Stream> stream(this->stream->Clone(), Acquire::NORETAIN);
    return new ModPlugDecoder(stream, this->bufferSize);
}

int ModPlugDecoder::Probe(Stream* stream)
{
    Data* data        = stream->Read(ModPlugDecoder::DATA_SIZE);
    ModPlugFile* plug = ModPlug_Load(data->GetData(), (int)data->GetSize());

    if (plug)
        ModPlug_Unload(plug);

    data->Release();

    return plug ? 80 : 0;
}

int ModPlugDecoder::Decode()
{
    return this->Decode((s16*)this->buffer);
}

int ModPlugDecoder::Decode(s16* buffer)
{
    int read = ModPlug_Read(this->plug, buffer, this->bufferSize);

    if (read == 0)
        this->eof = true;

    return read;
}

bool ModPlugDecoder::Seek(double position)
{
    ModPlug_Seek(this->plug, (int)(position * 1000.0));

    return true;
}

bool ModPlugDecoder::Rewind()
{
    // Let's reload.
    ModPlug_Unload(this->plug);

    this->plug = ModPlug_Load(this->data->GetData(), (int)data->GetSize());
    ModPlug_SetMasterVolume(this->plug, 128);

    this->eof = false;

    return (plug != 0);
}

bool ModPlugDecoder::IsSeekable()
{
    return true;
}

int ModPlugDecoder::GetChannelCount() const
{
    return 2;
}

int ModPlugDecoder::GetBitDepth() const
{
    return 16;
}

double ModPlugDecoder::GetDuration()
{
    // Only calculate the duration if we haven't done so already.
    if (this->duration == -2.0)
    {
        int lengthms = ModPlug_GetLength(this->plug);

        if (lengthms < 0)
            this->duration = -1.0;
        else
            this->duration = (double)lengthms / 1000.0;
    }

    return duration;
}
