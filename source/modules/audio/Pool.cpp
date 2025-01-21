#include "modules/audio/Pool.hpp"

namespace love
{
    Pool::Pool() : totalSources(0)
    {
        for (size_t index = 0; index < Pool::MAX_SOURCES; index++)
            this->available.push(index);

        this->totalSources = this->available.size();
    }

    Pool::~Pool()
    {
        Source::stop(this);
    }

    bool Pool::isAvailable()
    {
        bool has = false;

        {
            std::unique_lock lock(this->mutex);
            has = !this->available.empty();
        }

        return has;
    }

    bool Pool::isPlaying(Source* source)
    {
        bool playing = false;

        {
            std::unique_lock lock(this->mutex);
            playing = (this->playing.find(source) != this->playing.end());
        }

        return playing;
    }

    void Pool::update()
    {
        std::unique_lock lock(this->mutex);
        std::vector<Source*> release;

        for (const auto& iterator : this->playing)
        {
            if (!iterator.first->update())
                release.push_back(iterator.first);
        }

        for (auto* source : release)
            this->releaseSource(source);
    }

    int Pool::getActiveSourceCount() const
    {
        return this->playing.size();
    }

    int Pool::getMaxSources() const
    {
        return this->totalSources;
    }

    void Pool::addSource(Source* source, size_t channel)
    {
        this->playing.insert({ source, channel });
        source->retain();
    }

    bool Pool::assignSource(Source* source, size_t& channel, uint8_t& wasPlaying)
    {
        channel = 0;

        if (this->findSource(source, channel))
            return (wasPlaying = true);

        wasPlaying = false;

        if (this->available.empty())
            return false;

        channel = this->available.front();
        this->available.pop();

        return true;
    }

    bool Pool::releaseSource(Source* source, bool stop)
    {
        size_t channel;

        if (this->findSource(source, channel))
        {
            if (stop)
                source->stopAtomic();

            source->release();

            this->available.push(channel);
            this->playing.erase(source);

            return true;
        }

        return false;
    }

    bool Pool::findSource(Source* source, size_t& channel)
    {
        auto iterator = this->playing.find(source);

        if (iterator == this->playing.end())
            return false;

        channel = iterator->second;
        return true;
    }

    std::vector<Source*> Pool::getPlayingSources()
    {
        std::vector<Source*> sources;
        sources.reserve(this->playing.size());

        for (const auto& iterator : this->playing)
            sources.push_back(iterator.first);

        return sources;
    }

    std::unique_lock<std::recursive_mutex> Pool::lock()
    {
        return std::unique_lock(this->mutex);
    }
} // namespace love
