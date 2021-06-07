#include "modplugdecoder.h"

using namespace love;

ModPlugDecoder::ModPlugDecoder(Data* data, int bufferSize) :
    Decoder(data, bufferSize),
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
    this->plug = ModPlug_Load(data->GetData(), (int)data->GetSize());

    if (this->plug == NULL)
        throw love::Exception("Could not load file with ModPlug.");

    // set master volume for delicate ears
    ModPlug_SetMasterVolume(plug, 128);
}

ModPlugDecoder::~ModPlugDecoder()
{
    if (this->plug != 0)
        ModPlug_Unload(this->plug);
}

bool ModPlugDecoder::Accepts(const std::string& ext)
{
    static const std::string supported[] = { "699", "abc", "amf", "ams", "dbm", "dmf", "dsm",
                                             "far", "it",  "j2b", "mdl", "med", "mid", "mod",
                                             "mt2", "mtm", "okt", "pat", "psm", "s3m", "stm",
                                             "ult", "umx", "xm",  "" };

    for (int i = 0; !(supported[i].empty()); i++)
    {
        if (supported[i].compare(ext) == 0)
            return true;
    }

    return false;
}

Decoder* ModPlugDecoder::Clone()
{
    return new ModPlugDecoder(this->data.Get(), this->bufferSize);
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
