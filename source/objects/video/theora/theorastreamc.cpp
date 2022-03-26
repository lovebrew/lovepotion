#include "objects/video/theora/theorastreamc.h"
#include "objects/video/sync/deltasync.h"

#include "modules/thread/types/lock.h"

using namespace love::common;
using love::thread::Lock;

TheoraStream::TheoraStream(File* file) :
    demuxer(file),
    headerParsed(false),
    decoder(nullptr),
    frameReady(false),
    lastFrame(0),
    nextFrame(0)
{
    if (demuxer.FindStream() != OggDemuxer::TYPE_THEORA)
        throw love::Exception("Invalid video file, video is not theora.");

    this->frameSync.Set(new DeltaSync(), Acquire::NORETAIN);
}

TheoraStream::~TheoraStream()
{
    if (this->decoder)
        th_decode_free(this->decoder);

    th_info_clear(&this->info);
}

int TheoraStream::GetWidth() const
{
    if (this->headerParsed)
        return this->info.pic_width;

    return 0;
}

int TheoraStream::GetHeight() const
{
    if (this->headerParsed)
        return this->info.pic_height;

    return 0;
}

const std::string& TheoraStream::GetFilename() const
{
    return this->demuxer.GetFilename();
}

void TheoraStream::FillBackBuffer()
{}

void TheoraStream::SeekDecoder(double target)
{
    bool success = this->demuxer.Seek(packet, target, [this](int64_t granulepos) {
        return th_granule_time(this->decoder, granulepos);
    });

    if (!success)
        return;

    this->lastFrame = this->nextFrame = -1;
    th_decode_ctl(this->decoder, TH_DECCTL_SET_GRANPOS, &packet.granulepos,
                  sizeof(packet.granulepos));
}

void TheoraStream::SetSync(FrameSync* other)
{
    Lock lock(this->bufferMutex);
    this->frameSync = other;
}

bool TheoraStream::IsPlaying() const
{
    return this->frameSync->IsPlaying() && !this->demuxer.IsEOS();
}
