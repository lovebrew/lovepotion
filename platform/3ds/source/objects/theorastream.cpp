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
    this->frontBuffer = new Frame();
    this->backBuffer  = new Frame();

    th_info_init(&this->info);

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

size_t TheoraStream::GetSize() const
{
    return sizeof(Frame);
}

void TheoraStream::SetupBuffers()
{
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

    Frame* buffers[2] = { (Frame*)this->backBuffer, (Frame*)this->frontBuffer };

    for (int index = 0; index < 2; index++)
    {
        C3D_Tex* texture = buffers[index]->buffer;

        C3D_TexInit(texture, powTwoWidth, powTwoHeight, GPU_RGB8);
        C3D_TexSetFilter(texture, GPU_LINEAR, GPU_LINEAR);

        memset(texture->data, 0, texture->size);

        buffers[index]->width  = calcWidth;
        buffers[index]->height = calcHeight;
    }
}

void TheoraStream::SetPostProcessingLevel()
{
    if (this->quality.offset)
    {
        this->quality.current += this->quality.offset;
        th_decode_ctl(this->decoder, TH_DECCTL_SET_PPLEVEL, &this->quality.current,
                      sizeof(this->quality.current));
        this->quality.offset = 0;
    }
}

void TheoraStream::FillBufferData(th_ycbcr_buffer bufferInfo)
{
    bool isBusy = true;

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

    Y2RU_SetSendingU(bufferInfo[1].data, (this->width / 2) * (this->height / 2), this->width / 2,
                     bufferInfo[1].stride - (this->width >> 1));

    Y2RU_SetSendingV(bufferInfo[2].data, (this->width / 2) * (this->height / 2), this->width / 2,
                     bufferInfo[2].stride - (this->width >> 1));

    PixelFormat format;
    ::citro2d::GetConstant(((Frame*)this->backBuffer)->buffer->fmt, format);

    size_t formatSize = GetPixelFormatSize(format);

    Y2RU_SetReceiving(((Frame*)this->backBuffer)->buffer->data,
                      this->width * this->height * formatSize, this->width * 8 * formatSize,
                      (NextPO2(this->width) - this->width) * 8 * formatSize);

    /* convert the data */

    Y2RU_StartConversion();

    Y2RU_GetTransferEndEvent(&this->handle);
}
