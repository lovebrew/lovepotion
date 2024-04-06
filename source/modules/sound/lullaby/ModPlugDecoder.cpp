#include "modules/sound/lullaby/ModPlugDecoder.hpp"

#include "common/Data.hpp"
#include "common/Exception.hpp"

namespace love
{
    ModPlugDecoder::ModPlugDecoder(Stream* stream, int bufferSize) :
        Decoder(stream, bufferSize),
        handle(nullptr),
        duration(-2.0)
    {
        this->settings.mFlags     = MODPLUG_ENABLE_OVERSAMPLING | MODPLUG_ENABLE_NOISE_REDUCTION;
        this->settings.mChannels  = 2;
        this->settings.mBits      = 16;
        this->settings.mFrequency = sampleRate;
        this->settings.mResamplingMode = MODPLUG_RESAMPLE_LINEAR;

        this->settings.mStereoSeparation = 128;
        this->settings.mMaxMixChannels   = 32;
        this->settings.mReverbDepth      = 0;
        this->settings.mReverbDelay      = 0;
        this->settings.mBassAmount       = 0;
        this->settings.mBassRange        = 0;
        this->settings.mSurroundDepth    = 0;
        this->settings.mSurroundDelay    = 0;
        this->settings.mLoopCount        = -1;

        ModPlug_SetSettings(&this->settings);

        /*
        ** ModPlug has no streaming API. Miserable.
        ** We don't want to load the entire stream immediately if it's big, because
        ** it might not be compatible with ModPlug. So we just try to load 4MB and
        ** see if that works, and then load the whole thing if it does.
        */
        if (stream->getSize() > 1024 * 1024 * 4)
        {
            this->data.set(stream->read(1024 * 1024 * 4), Acquire::NO_RETAIN);
            this->handle = ModPlug_Load(this->data->getData(), (int)this->data->getSize());

            if (this->handle == nullptr)
                throw love::Exception("Could not load file with ModPlug.");

            stream->seek(0);
            ModPlug_Unload(this->handle);
        }

        this->data.set(stream->read(stream->getSize()), Acquire::NO_RETAIN);
        this->handle = ModPlug_Load(this->data->getData(), (int)this->data->getSize());

        if (this->handle == nullptr)
            throw love::Exception("Could not load file with ModPlug.");

        ModPlug_SetMasterVolume(this->handle, 128);
    }

    ModPlugDecoder::~ModPlugDecoder()
    {
        if (this->handle != nullptr)
            ModPlug_Unload(this->handle);
    }

    Decoder* ModPlugDecoder::clone()
    {
        StrongRef<Stream> _stream(this->stream->clone(), Acquire::NO_RETAIN);
        return new ModPlugDecoder(_stream, this->bufferSize);
    }

    int ModPlugDecoder::decode()
    {
        auto read = ModPlug_Read(this->handle, this->buffer, this->bufferSize);

        if (read == 0)
            this->eof = true;

        return read;
    }

    bool ModPlugDecoder::seek(double position)
    {
        ModPlug_Seek(this->handle, (int)(position * 1000.0));

        return true;
    }

    bool ModPlugDecoder::rewind()
    {
        ModPlug_Unload(this->handle);
        this->handle = ModPlug_Load(this->data->getData(), (int)this->data->getSize());

        ModPlug_SetMasterVolume(this->handle, 128);
        this->eof = false;

        return this->handle != nullptr;
    }

    bool ModPlugDecoder::isSeekable()
    {
        return true;
    }

    int ModPlugDecoder::getChannelCount() const
    {
        return 2;
    }

    int ModPlugDecoder::getBitDepth() const
    {
        return 16;
    }

    double ModPlugDecoder::getDuration()
    {
        if (this->duration == -2.0)
        {
            int lengthMs = ModPlug_GetLength(this->handle);

            if (lengthMs < 0)
                this->duration = -1.0;
            else
                this->duration = lengthMs / 1000.0;
        }

        return this->duration;
    }
} // namespace love
