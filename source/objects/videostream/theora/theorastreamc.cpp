#include "objects/videostream/theora/theorastreamc.h"
#include "objects/videostream/sync/deltasync.h"

#include "modules/thread/types/lock.h"

using namespace love::common;
using love::thread::Lock;

TheoraStream::TheoraStream(File* file) :
    demuxer(file),
    headerParsed(false),
    decoder(nullptr),
    frameReady(false),
    lastFrame(0),
    nextFrame(0),
    quality {}
{
    if (demuxer.FindStream() != OggDemuxer::TYPE_THEORA)
        throw love::Exception("Invalid video file, video is not theora.");

    this->frameSync.Set(new DeltaSync(), Acquire::NORETAIN);
}

TheoraStream::~TheoraStream()
{
    if (this->decoder)
        th_decode_free(this->decoder);

    delete this->frontBuffer;
    delete this->backBuffer;

    th_info_clear(&this->info);
}

void TheoraStream::ParseHeader()
{
    if (this->headerParsed)
        return;

    th_comment comment;
    th_setup_info* setupInfo = nullptr;

    th_comment_init(&comment);
    int result = 0;

    this->demuxer.ReadPacket(this->packet);
    result = th_decode_headerin(&this->info, &comment, &setupInfo, &this->packet);

    if (result < 0)
    {
        th_comment_clear(&comment);
        throw love::Exception("Could not find theora header.");
    }

    while (result > 0)
    {
        this->demuxer.ReadPacket(this->packet);
        result = th_decode_headerin(&this->info, &comment, &setupInfo, &this->packet);
    }

    th_comment_clear(&comment);

    this->decoder = th_decode_alloc(&this->info, setupInfo);
    th_setup_free(setupInfo);

    /* set up buffers in subclass */
    this->SetupBuffers();

    this->headerParsed = true;
    th_decode_packetin(this->decoder, &this->packet, nullptr);
}

void TheoraStream::ThreadedFillBackBuffer(double dt)
{
    this->frameSync->Update(dt);
    double position = this->frameSync->GetPosition();

    if (position < lastFrame)
        this->SeekDecoder(position);

    th_ycbcr_buffer bufferInfo;
    bool hasFrame = false;

    /*
    ** until we are at the end of the stream
    ** or we are displaying the right frame
    */

    size_t framesBehind = 0;
    bool failedSeek     = false;

    while (!this->demuxer.IsEOS() && position >= this->nextFrame)
    {
        if (framesBehind++ > 5 && !failedSeek)
        {
            this->SeekDecoder(position);
            framesBehind = 0;
            failedSeek   = true;
        }

        if (failedSeek)
            this->quality.offset = std::max(this->quality.current - 1, 0);
        else
        {
            if (framesBehind == 0)
                this->quality.offset = std::min(this->quality.offset + 1, this->quality.maximum);
        }

        // this->SetPostProcessingLevel();

        th_decode_ycbcr_out(this->decoder, bufferInfo);
        hasFrame = true;

        ogg_int64_t granulePosition = -1;

        do
        {
            if (this->demuxer.ReadPacket(this->packet))
                return;
        } while (th_decode_packetin(this->decoder, &this->packet, &granulePosition) != 0);

        this->lastFrame = this->nextFrame;
        this->nextFrame = th_granule_time(this->decoder, granulePosition);
    }

    /* only swap once, even if we read many frames to get here */

    if (hasFrame)
    {
        /* don't swap whilst we're writing to the backbuffer */

        {
            thread::Lock lock(this->bufferMutex);
            this->frameReady = false;
        }

        if (!bufferInfo[0].data || !bufferInfo[1].data || !bufferInfo[2].data)
            return;

        /* fill the buffer data, handled in the subclass */
        this->FillBufferData(bufferInfo);

        /* re-enable swapping */

        {
            thread::Lock lock(this->bufferMutex);
            this->frameReady = true;
        }
    }
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

const void* TheoraStream::GetFrontBuffer() const
{
    return this->frontBuffer;
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

bool TheoraStream::SwapBuffers()
{
    if (this->demuxer.IsEOS())
        return false;

    if (!this->frameSync->IsPlaying())
        return false;

    thread::Lock lock(this->bufferMutex);

    if (!this->frameReady)
        return false;

    this->frameReady = false;

    IFrame* temp = this->frontBuffer;

    this->frontBuffer = this->backBuffer;
    this->backBuffer  = temp;

    return true;
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
