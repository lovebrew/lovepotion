#include "common/lmath.h"

#include "modules/thread/types/lock.h"
#include "objects/videostream/theorastream.h"

#include "citro2d/citro.h"
#include "common/pixelformat.h"

using namespace love;

TheoraStream::Frame::Frame()
{
    this->buffer = new C3D_Tex();
}

TheoraStream::Frame::~Frame()
{
    C3D_TexDelete(this->buffer);
    delete this->buffer;
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
        delete this->frontBuffer;
        delete this->backBuffer;

        th_info_clear(&this->info);

        throw exception;
    }
}

TheoraStream::~TheoraStream()
{
    delete this->frontBuffer;
    delete this->backBuffer;
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

    switch (this->info.pixel_fmt)
    {
        case TH_PF_420:
            break;
        case TH_PF_422:
            break;
        case TH_PF_444:
            throw love::Exception("YUV444 is not supported by Y2R");
            return;
        case TH_PF_RSVD:
        default:
            throw love::Exception("UNKNOWN Chroma sampling!");
            return;
    }

    this->format = this->info.pixel_fmt;

    int calcWidth =
        ((this->info.pic_x + this->info.frame_width + 1) & ~1) - (this->info.pic_x & ~1);
    int calcHeight =
        ((this->info.pic_y + this->info.frame_height + 1) & ~1) - (this->info.pic_y & ~1);

    this->width  = calcWidth;
    this->height = calcHeight;

    int powTwoWidth  = NextPO2(calcWidth);
    int powTwoHeight = NextPO2(calcHeight);

    Frame* buffers[2] = { this->backBuffer, this->frontBuffer };

    for (int index = 0; index < 2; index++)
    {
        C3D_Tex* texture = buffers[index]->buffer;

        C3D_TexInit(texture, powTwoWidth, powTwoHeight, GPU_RGB8);
        C3D_TexSetFilter(texture, GPU_LINEAR, GPU_LINEAR);

        memset(texture->data, 0, texture->size);

        buffers[index]->width  = calcWidth;
        buffers[index]->height = calcHeight;
    }

    this->headerParsed = true;
    th_decode_packetin(this->decoder, &this->packet, nullptr);
}

void TheoraStream::SetPostProcessingLevel()
{
    if (this->postProcessOffset)
    {
        this->postProcess += this->postProcessOffset;
        th_decode_ctl(this->decoder, TH_DECCTL_SET_PPLEVEL, &this->postProcess,
                      sizeof(this->postProcess));
        this->postProcessOffset = 0;
    }
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
            this->postProcessOffset = this->postProcess > 0 ? -1 : 0;
        else
        {
            if (framesBehind == 0)
                this->postProcessOffset = this->postProcess < this->maxPostProcess ? 1 : 0;
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

        bool isBusy = true;

        if (!bufferInfo[0].data || !bufferInfo[1].data || !bufferInfo[2].data)
            return;

        Y2RU_StopConversion();

        while (isBusy)
            Y2RU_IsBusyConversion(&isBusy);

        switch (this->format)
        {
            case TH_PF_420:
                Y2RU_SetInputFormat(Y2RU_InputFormat::INPUT_YUV420_INDIV_8);
                break;
            case TH_PF_422:
                Y2RU_SetInputFormat(Y2RU_InputFormat::INPUT_YUV422_INDIV_8);
                break;
            default:
                break;
        }

        Y2RU_SetOutputFormat(Y2RU_OutputFormat::OUTPUT_RGB_24);
        Y2RU_SetRotation(Y2RU_Rotation::ROTATION_NONE);
        Y2RU_SetBlockAlignment(Y2RU_BlockAlignment::BLOCK_8_BY_8);
        Y2RU_SetTransferEndInterrupt(true);
        Y2RU_SetInputLineWidth(this->width);
        Y2RU_SetInputLines(this->height);
        Y2RU_SetStandardCoefficient(Y2RU_StandardCoefficient::COEFFICIENT_ITU_R_BT_601_SCALING);
        Y2RU_SetAlpha(0xFF);

        /* set up the YUV data for Y2RU */

        Y2RU_SetSendingY(bufferInfo[0].data, this->width * this->height, this->width,
                         bufferInfo[0].stride - this->width);

        Y2RU_SetSendingU(bufferInfo[1].data, (this->width / 2) * (this->height / 2),
                         this->width / 2, bufferInfo[1].stride - (this->width >> 1));

        Y2RU_SetSendingV(bufferInfo[2].data, (this->width / 2) * (this->height / 2),
                         this->width / 2, bufferInfo[2].stride - (this->width >> 1));

        PixelFormat format;
        ::citro2d::GetConstant(this->backBuffer->buffer->fmt, format);

        size_t formatSize = GetPixelFormatSize(format);

        Y2RU_SetReceiving(this->backBuffer->buffer->data, this->width * this->height * formatSize,
                          this->width * 8 * formatSize,
                          (NextPO2(this->width) - this->width) * 8 * formatSize);

        /* convert the data */

        Y2RU_StartConversion();

        Y2RU_GetTransferEndEvent(&this->handle);

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
