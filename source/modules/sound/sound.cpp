#include "common/runtime.h"
#include "modules/sound/sound.h"

using namespace love;

love::Type Sound::type("Sound", &Module::type);

struct DecoderImpl
{
    Decoder *(*Create)(FileData * data, int bufferSize);
    bool (*Accepts)(const std::string & ext);
};

template<typename DecoderType>
DecoderImpl DecoderImplFor()
{
    DecoderImpl decoderImpl;

    decoderImpl.Create = [](FileData * data, int bufferSize) -> Decoder *
    {
        return new DecoderType(data, bufferSize);
    };

    decoderImpl.Accepts = [](const std::string & ext) -> bool
    {
        return DecoderType::Accepts(ext);
    };

    return decoderImpl;
}

Sound::~Sound()
{}

Decoder * Sound::NewDecoder(FileData * data, int bufferSize)
{
    std::string ext = data->GetExtension();
    std::transform(ext.begin(), ext.end(), ext.begin(), tolower);

    std::vector<DecoderImpl> possibilities = {
        DecoderImplFor<VorbisDecoder>()
    };

    for (DecoderImpl & item : possibilities)
    {
        if (item.Accepts(ext))
            return item.Create(data, bufferSize);
    }

    return nullptr;
}

SoundData * Sound::NewSoundData(Decoder * decoder)
{
    return new SoundData(decoder);
}
