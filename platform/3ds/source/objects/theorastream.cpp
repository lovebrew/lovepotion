#include "common/lmath.h"

#include "modules/thread/types/lock.h"
#include "objects/video/theorastream.h"

using namespace love;

TheoraStream::TheoraStream(File* file) : common::TheoraStream(file)
{
    th_info_init(&this->info);

    this->frame = new Frame();

    try
    {
        this->ParseHeader();
    }
    catch (love::Exception& exception)
    {
        delete this->frame;

        th_info_clear(&this->info);

        throw exception;
    }
}

const void* TheoraStream::GetFrontBuffer() const
{
    return this->frame;
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
    result = th_decode_headerin(&this->info, &comment, &setupInfo, &packet);

    if (result < 0)
    {
        th_comment_clear(&comment);
        throw love::Exception("Could not find theora header.");
    }

    while (result > 0)
    {
        this->demuxer.ReadPacket(packet);
        result = th_decode_headerin(&this->info, &comment, &setupInfo, &packet);
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
            /* YUV444 is not supported by Y2R */
            return;
        case TH_PF_RSVD:
        default:
            /* UNKNOWN Chroma sampling! */
            return;
    }

    this->frame->format = this->info.pixel_fmt;

    int width  = ((this->info.pic_x + this->info.frame_width + 1) & ~1) - (this->info.pic_x & ~1);
    int height = ((this->info.pic_y + this->info.frame_height + 1) & ~1) - (this->info.pic_y & ~1);

    this->frame->width  = width;
    this->frame->height = height;

    int powTwoWidth  = NextPO2(width);
    int powTwoHeight = NextPO2(height);

    for (int index = 0; index < 2; index++)
    {
        C3D_Tex* curtex = &this->frame->buffer[index];

        C3D_TexInit(curtex, width, height, GPU_RGB8);
        C3D_TexSetFilter(curtex, GPU_LINEAR, GPU_LINEAR);

        memset(curtex->data, 0, curtex->size);
    }

    this->subTexture.width  = width;
    this->subTexture.height = height;

    this->subTexture.left = 0.0f;
    this->subTexture.top  = 1.0f;

    this->subTexture.right  = (float)width / powTwoWidth;
    this->subTexture.bottom = 1.0f - ((float)height / powTwoHeight);

    this->headerParsed = true;
    th_decode_packetin(this->decoder, &packet, nullptr);
}

/* to do.. do not do this */
static inline size_t getFormatComponents(GPU_TEXCOLOR fmt)
{
    switch (fmt)
    {
        case GPU_RGBA8:
            return 4;
        case GPU_RGB8:
            return 3;
        default:
            return 0;
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

        th_decode_ycbcr_out(decoder, bufferInfo);
        hasFrame = true;

        ogg_int64_t granulePosition = -1;
        do
        {
            if (this->demuxer.ReadPacket(packet))
                return;
        } while (th_decode_packetin(this->decoder, &packet, &granulePosition) != 0);

        this->lastFrame = this->nextFrame;
        this->nextFrame = th_granule_time(this->decoder, granulePosition);
    }

    if (hasFrame)
    {
        {
            thread::Lock lock(this->bufferMutex);
            this->frameReady = false;
        }

        bool isBusy     = true;
        bool drawBuffer = !this->frame->currentBuffer;

        C3D_Tex* writeFrame = &this->frame->buffer[drawBuffer];
        if (!bufferInfo[0].data || !bufferInfo[1].data || !bufferInfo[2].data)
            return;

        Y2RU_StopConversion();

        if (isBusy)
            Y2RU_IsBusyConversion(&isBusy);

        switch (this->frame->format)
        {
            case TH_PF_420:
                Y2RU_SetInputFormat(Y2RU_InputFormat::INPUT_YUV420_INDIV_8);
                break;
            case TH_PF_422:
            default:
                Y2RU_SetInputFormat(Y2RU_InputFormat::INPUT_YUV422_INDIV_8);
                break;
        }

        Y2RU_SetOutputFormat(Y2RU_OutputFormat::OUTPUT_RGB_24);
        Y2RU_SetRotation(Y2RU_Rotation::ROTATION_NONE);
        Y2RU_SetBlockAlignment(Y2RU_BlockAlignment::BLOCK_8_BY_8);
        Y2RU_SetTransferEndInterrupt(true);
        Y2RU_SetInputLineWidth(this->frame->width);
        Y2RU_SetInputLines(this->frame->height);
        Y2RU_SetStandardCoefficient(Y2RU_StandardCoefficient::COEFFICIENT_ITU_R_BT_601_SCALING);
        Y2RU_SetAlpha(0xFF);

        /* set up the YUV datafor Y2RU */

        Y2RU_SetSendingY(bufferInfo[0].data, this->frame->width * this->frame->height,
                         this->frame->width, bufferInfo[0].stride - this->frame->width);

        Y2RU_SetSendingU(bufferInfo[1].data, (this->frame->width / 2) * (this->frame->height / 2),
                         this->frame->width / 2, bufferInfo[1].stride - (this->frame->width >> 1));

        Y2RU_SetSendingV(bufferInfo[2].data, (this->frame->width / 2) * (this->frame->height / 2),
                         this->frame->width / 2, bufferInfo[2].stride - (this->frame->width >> 1));

        size_t formatSize = getFormatComponents(writeFrame->fmt);

        Y2RU_SetReceiving(writeFrame->data, this->frame->width * this->frame->height * formatSize,
                          this->frame->width * 8 * formatSize,
                          (NextPO2(this->frame->width) - this->frame->width) * 8 * formatSize);

        /* convert the data */

        Y2RU_StartConversion();

        Y2RU_GetTransferEndEvent(&this->handle);

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

    this->frame->currentBuffer = !this->frame->currentBuffer;

    return true;
}
