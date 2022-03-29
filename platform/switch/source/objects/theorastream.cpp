#include "objects/videostream/theorastream.h"

#include "modules/thread/types/lock.h"

using namespace love;

TheoraStream::Frame::Frame() : yPlane(nullptr), cbPlane(nullptr), crPlane(nullptr)
{}

TheoraStream::Frame::~Frame()
{
    delete[] this->yPlane;
    delete[] this->cbPlane;
    delete[] this->crPlane;
}

TheoraStream::TheoraStream(File* file) : common::TheoraStream(file)
{
    th_info_init(&this->info);

    this->frontBuffer = new Frame();
    this->backBuffer  = new Frame();

    try
    {
        this->ParseHeader();
    }
    catch (love::Exception& exception)
    {
        delete this->backBuffer;
        delete this->frontBuffer;

        th_info_clear(&this->info);

        throw exception;
    }
}

TheoraStream::~TheoraStream()
{
    if (this->decoder)
        th_decode_free(this->decoder);

    th_info_clear(&this->info);

    delete this->frontBuffer;
    delete this->backBuffer;
}

template<typename T>
inline void scaleFormat(th_pixel_fmt format, T& x, T& y)
{
    switch (format)
    {
        case TH_PF_420:
            y /= 2;
        case TH_PF_422:
            x /= 2;
            break;
        default:
            break;
    }
}

const void* TheoraStream::GetFrontBuffer() const
{
    return this->frontBuffer;
}

size_t TheoraStream::GetSize() const
{
    return sizeof(Frame);
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

    Frame* buffers[2] = { this->backBuffer, this->frontBuffer };

    this->yPlaneXOffset = this->cPlaneXOffset = this->info.pic_x;
    this->yPlaneYOffset = this->cPlaneYOffset = this->info.pic_y;

    scaleFormat(this->info.pixel_fmt, cPlaneXOffset, cPlaneYOffset);

    for (size_t index = 0; index < 2; index++)
    {
        buffers[index]->cw = buffers[index]->yw = this->info.pic_width;
        buffers[index]->ch = buffers[index]->yh = this->info.pic_height;

        scaleFormat(this->info.pixel_fmt, buffers[index]->cw, buffers[index]->ch);

        buffers[index]->yPlane  = new uint8_t[buffers[index]->yw * buffers[index]->yh];
        buffers[index]->cbPlane = new uint8_t[buffers[index]->cw * buffers[index]->ch];
        buffers[index]->crPlane = new uint8_t[buffers[index]->cw * buffers[index]->ch];

        memset(buffers[index]->yPlane, 16, buffers[index]->yw * buffers[index]->yh);
        memset(buffers[index]->cbPlane, 128, buffers[index]->cw * buffers[index]->ch);
        memset(buffers[index]->crPlane, 128, buffers[index]->cw * buffers[index]->ch);
    }

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

        // this->SetPostProcessingLevel();

        th_decode_ycbcr_out(this->decoder, bufferInfo);
        hasFrame = true;

        ogg_int64_t granulePosition = -1;
        int result                  = 0;

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

        for (int y = 0; y < this->backBuffer->yh; ++y)
        {
            memcpy(this->backBuffer->yPlane + this->backBuffer->yw * y,
                   bufferInfo[0].data + bufferInfo[0].stride * (y + yPlaneYOffset) + yPlaneXOffset,
                   this->backBuffer->yw);
        }

        for (int y = 0; y < this->backBuffer->ch; ++y)
        {
            memcpy(this->backBuffer->cbPlane + this->backBuffer->cw * y,
                   bufferInfo[1].data + bufferInfo[1].stride * (y + cPlaneYOffset) + cPlaneXOffset,
                   this->backBuffer->cw);
        }

        for (int y = 0; y < this->backBuffer->ch; ++y)
        {
            memcpy(backBuffer->crPlane + backBuffer->cw * y,
                   bufferInfo[2].data + bufferInfo[2].stride * (y + cPlaneYOffset) + cPlaneXOffset,
                   this->backBuffer->cw);
        }

        /* re-enable swapping */

        {
            thread::Lock lock(this->bufferMutex);
            this->frameReady = true;
        }
    }
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

    Frame* temp = this->frontBuffer;

    this->frontBuffer = this->backBuffer;
    this->backBuffer  = temp;

    return true;
}
