#include <objects/imagedata/imagedata.tcc>

#include <modules/image/imagemodule.hpp>

using namespace love;

template<>
void ImageData<Console::ALL>::Decode(Data* data)
{
    FormatHandler* decoder = nullptr;
    FormatHandler::DecodedImage image {};

    auto module = Module::GetInstance<ImageModule>(Module::M_IMAGE);

    if (module == nullptr)
        throw love::Exception("love.image must be loaded in order to decode an ImageData.");

    for (auto* handler : module->GetFormatHandlers())
    {
        if (handler->CanDecode(data))
        {
            decoder = handler;
            break;
        }
    }

    if (decoder)
        image = decoder->Decode(data);

    if (image.data == nullptr)
    {
        auto* fileData = (FileData*)data;

        if (fileData != nullptr)
        {
            const auto& name = fileData->GetFilename();
            throw love::Exception(
                "Could not decode file '%s' to ImageData: unsupported file format", name.c_str());
        }
        else
        {
            throw love::Exception("Could not decode data to ImageData: unsupported "
                                  "encoded format");
        }
    }

    const auto sliceSize = love::GetPixelFormatSliceSize(image.format, image.width, image.height);

    if (image.size != sliceSize)
        throw love::Exception("Could not convert image!");

    this->width  = image.width;
    this->height = image.height;
    this->data   = std::move(image.data);
    this->format = image.format;

    this->decoder = decoder;

    this->pixelSetFunction = GetPixelSetFunction(this->format);
    this->pixelGetFunction = GetPixelGetFunction(this->format);
}