#include <utilities/pool/sources.hpp>

using namespace love;

AudioPool::AudioPool() : totalSources(0)
{
    for (size_t index = 0; index < AudioPool::MAX_SOURCES; index++)
        this->available.push(index);

    this->totalSources = this->available.size();
}

AudioPool::~AudioPool()
{
    Source<Console::Which>::Stop(this);
}

bool AudioPool::IsAvailable()
{
    bool has = false;

    {
        std::unique_lock lock(this->mutex);
        has = !this->available.empty();
    }

    return has;
}

bool AudioPool::IsPlaying(Source<Console::Which>* source)
{
    bool playing = false;

    {
        std::unique_lock lock(this->mutex);
        playing = (this->playing.find(source) != this->playing.end());
    }

    return playing;
}

void AudioPool::Update()
{
    std::unique_lock lock(this->mutex);
    std::vector<Source<Console::Which>*> release;

    for (const auto& iterator : this->playing)
    {
        if (!iterator.first->Update())
            release.push_back(iterator.first);
    }

    for (auto* source : release)
        this->ReleaseSource(source);
}

std::unique_lock<love::mutex> AudioPool::Lock()
{
    std::unique_lock lock(this->mutex);

    return lock;
}

int AudioPool::GetActiveSourceCount() const
{
    return this->playing.size();
}

int AudioPool::GetMaxSources() const
{
    return this->totalSources;
}

void AudioPool::AddSource(Source<Console::Which>* source, size_t channel)
{
    this->playing.insert(std::make_pair(source, channel));
    source->Retain();
}

bool AudioPool::AssignSource(Source<Console::Which>* source, size_t& channel, uint8_t& wasPlaying)
{
    channel = 0;

    if (this->FindSource(source, channel))
        return (wasPlaying = true);

    wasPlaying = false;

    if (this->available.empty())
        return false;

    channel = this->available.front();
    this->available.pop();

    if (!Console::Is(Console::CTR))
        this->AddSource(source, channel);

    return true;
}

bool AudioPool::ReleaseSource(Source<Console::Which>* source, bool stop)
{
    size_t channel;

    if (this->FindSource(source, channel))
    {
        if (stop)
            source->StopAtomic();

        source->Release();

        this->available.push(channel);
        this->playing.erase(source);

        return true;
    }

    return false;
}

bool AudioPool::FindSource(Source<Console::Which>* source, size_t& channel)
{
    const auto iterator = this->playing.find(source);

    if (iterator == this->playing.end())
        return false;

    channel = iterator->second;
    return true;
}

std::vector<Source<Console::Which>*> AudioPool::GetPlayingSources()
{
    std::vector<Source<Console::Which>*> sources;
    sources.reserve(this->playing.size());

    for (auto& iterator : this->playing)
        sources.push_back(iterator.first);

    return sources;
}
