#include <modules/image/imagemodule.hpp>
#include <objects/imagedata_ext.hpp>

using namespace love;

void ImageData<Console::CTR>::Paste(ImageData* source, int x, int y, Rect& sourceRect)
{
    PixelFormat destFormat = this->GetFormat();
    PixelFormat srcFormat  = source->GetFormat();

    const auto srcWidth  = source->GetWidth();
    const auto srcHeight = source->GetHeight();

    const auto destWidth  = this->GetWidth();
    const auto destHeight = this->GetHeight();

    size_t srcPixelSize = source->GetPixelSize();
    size_t dstPixelSize = this->GetPixelSize();

    this->AdjustPaste(source, x, y, destWidth, destHeight, sourceRect);

    std::unique_lock lock(source->mutex);
    std::unique_lock other(this->mutex);

    uint8_t* srcData = (uint8_t*)source->GetData();
    uint8_t* dstData = (uint8_t*)this->GetData();

    auto getFunction = source->pixelGetFunction;
    auto setFunction = this->pixelSetFunction;

    const auto _srcWidth = NextPo2(srcWidth);
    const auto _dstWidth = NextPo2(destWidth);

    for (int _y = 0; _y < std::min(sourceRect.h, destHeight - y); _y++)
    {
        for (int _x = 0; _x < std::min(sourceRect.w, destWidth - x); _x++)
        {
            Color color {};

            // clang-format off
            Vector2 srcPosition { (sourceRect.x + _x), (sourceRect.y + _y) };
            const auto* sourcePixel = Color::FromTile(srcData, _srcWidth, srcPosition);

            getFunction((const Pixel*)sourcePixel, color);

            Vector2 dstPosition { (x + _x), (y + _y) };
            auto* destinationPixel = Color::FromTile(dstData, _dstWidth, dstPosition);

            setFunction(color, (Pixel*)destinationPixel);
            // clang-format on
        }
    }
}