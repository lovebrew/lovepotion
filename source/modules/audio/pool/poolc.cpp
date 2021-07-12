#include "modules/audio/pool/pool.h"

#include "objects/source/source.h"

using namespace love;

Pool::Pool()
{
    for (size_t channel = 0; channel < this->TOTAL_CHANNELS; channel++)
        this->available.push(channel);
}

Pool::~Pool()
{
    Source::Stop(this);
}

int Pool::GetActiveSourceCount() const
{
    return (int)this->playing.size();
}

int Pool::GetMaxSources() const
{
    return this->TOTAL_CHANNELS;
}

std::vector<common::Source*> Pool::GetPlayingSources()
{
    std::vector<common::Source*> sources;
    sources.reserve(this->playing.size());

    for (auto& item : this->playing)
        sources.push_back(item.first);

    return sources;
}

bool Pool::IsRunning()
{
    return this->running;
}

void Pool::Finish()
{
    this->running = false;
}

bool Pool::IsPlaying(common::Source* source)
{
    return (this->playing.find(source) != this->playing.end());
}

bool Pool::AssignSource(common::Source* source, size_t& channel, char& wasPlaying)
{
    channel = 0;

    if (this->FindSource(source, channel))
        return (wasPlaying = true);

    wasPlaying = false;

    if (this->available.empty())
        return false;

    channel = this->available.front();

    this->available.pop();

    return true;
}

void Pool::AddSource(common::Source* source, size_t channel)
{
    this->playing.insert(std::make_pair(source, channel));
    source->Retain();
}

bool Pool::FindSource(common::Source* source, size_t& channel)
{
    auto iterator = this->playing.find(source);

    if (iterator == this->playing.end())
        return false;

    channel = iterator->second;

    return true;
}

bool Pool::ReleaseSource(common::Source* source, bool stop)
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

void Pool::Update()
{
    thread::Lock lock(this->mutex);

    std::vector<common::Source*> release;

    for (const auto& item : this->playing)
    {
        if (!item.first->Update())
            release.push_back(item.first);
    }

    for (common::Source* source : release)
        this->ReleaseSource(source);
}

thread::Lock Pool::Lock()
{
    return thread::Lock(this->mutex);
}
