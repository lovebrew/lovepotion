#include "common/runtime.h"

#include "objects/source/source.h"
#include "modules/audio/pool/pool.h"

using namespace love;

Pool::Pool()
{
    for (size_t channel = 0; channel < this->TOTAL_CHANNELS; channel++)
        this->available.push(channel);
}

bool Pool::IsRunning()
{
    return this->running;
}

void Pool::Finish()
{
    this->running = false;
}

bool Pool::IsPlaying(Source * source)
{
    bool isPlaying = false;

    {
        thread::Lock lock(this->mutex);
        isPlaying = (this->playing.find(source) != this->playing.end());
    }

    return isPlaying;
}

bool Pool::AssignSource(Source * source, size_t & channel, bool & wasPlaying)
{
    channel = 0;

    if (this->FindSource(source, channel))
        return wasPlaying = true;

    wasPlaying = false;

    if (this->available.empty())
        return false;
    channel = this->available.front();

    this->available.pop();

    this->playing.insert(std::make_pair(source, channel));
    source->Retain();

    return true;
}

bool Pool::FindSource(Source * source, size_t & channel)
{
    auto iterator = this->playing.find(source);

    if (iterator == this->playing.end())
        return false;

    channel = iterator->second;

    return true;
}

bool Pool::ReleaseSource(Source * source, bool stop)
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

    std::vector<Source *> release;

    for (const auto & item : this->playing)
    {
        if (!item.first->Update())
            release.push_back(item.first);
    }

    for (Source * source : release)
        this->ReleaseSource(source);
}

thread::Lock Pool::Lock()
{
    return thread::Lock(this->mutex);
}
