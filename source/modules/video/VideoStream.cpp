#include "modules/video/VideoStream.hpp"

namespace love
{
    Type VideoStream::type("VideoStream", &Object::type);

    void VideoStream::setSync(FrameSync* sync)
    {
        this->frameSync = sync;
    }

    VideoStream::FrameSync* VideoStream::getSync() const
    {
        return this->frameSync;
    }

    void VideoStream::play()
    {
        this->frameSync->play();
    }

    void VideoStream::pause()
    {
        this->frameSync->pause();
    }

    void VideoStream::seek(double offset)
    {
        this->frameSync->seek(offset);
    }

    double VideoStream::tell() const
    {
        return this->frameSync->tell();
    }

    bool VideoStream::isPlaying() const
    {
        return this->frameSync->isPlaying();
    }

    VideoStream::Frame::Frame() : yPlane(nullptr), cbPlane(nullptr), crPlane(nullptr)
    {}

    VideoStream::Frame::~Frame()
    {
        delete[] this->yPlane;
        delete[] this->cbPlane;
        delete[] this->crPlane;
        delete[] this->data;
    }

    void VideoStream::FrameSync::copyState(const FrameSync* other)
    {
        this->seek(other->tell());
        if (other->isPlaying())
            this->play();
        else
            this->pause();
    }

    double VideoStream::FrameSync::tell() const
    {
        return this->getPosition();
    }

    VideoStream::DeltaSync::DeltaSync() : playing(false), position(0.0), speed(1.0)
    {}

    VideoStream::DeltaSync::~DeltaSync()
    {}

    double VideoStream::DeltaSync::getPosition() const
    {
        return this->position;
    }

    void VideoStream::DeltaSync::update(double dt)
    {
        std::unique_lock lock(this->mutex);
        if (this->playing)
            this->position += dt * this->speed;
    }

    void VideoStream::DeltaSync::play()
    {
        this->playing = true;
    }

    void VideoStream::DeltaSync::pause()
    {
        this->playing = false;
    }

    void VideoStream::DeltaSync::seek(double offset)
    {
        std::unique_lock lock(this->mutex);
        this->position = offset;
    }

    bool VideoStream::DeltaSync::isPlaying() const
    {
        return this->playing;
    }

    VideoStream::SourceSync::SourceSync(SourceBase* source) : source(source)
    {}

    double VideoStream::SourceSync::getPosition() const
    {
        return this->source->tell(SourceBase::UNIT_SECONDS);
    }

    void VideoStream::SourceSync::play()
    {
        this->source->play();
    }

    void VideoStream::SourceSync::pause()
    {
        this->source->pause();
    }

    void VideoStream::SourceSync::seek(double offset)
    {
        this->source->seek(offset, SourceBase::UNIT_SECONDS);
    }

    bool VideoStream::SourceSync::isPlaying() const
    {
        return this->source->isPlaying();
    }
} // namespace love
