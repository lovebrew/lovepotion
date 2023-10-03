#include <objects/imagedata/imagedata.tcc>

#include <modules/filesystem/physfs/filesystem.hpp>
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

    const auto sliceSize =
        love::GetPixelFormatSliceSize(image.format, image.width, image.height, false);

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

template<>
FileData* ImageData<Console::ALL>::Encode(FormatHandler::EncodedFormat encodedFormat,
                                          const char* filename, bool writeFile) const
{
    FormatHandler* encoder = nullptr;
    FormatHandler::EncodedImage image {};
    FormatHandler::DecodedImage rawImage {};

    rawImage.width  = width;
    rawImage.height = height;
    rawImage.size   = this->GetSize();

    rawImage.data = std::make_unique<uint8_t[]>(rawImage.size);
    std::memcpy(rawImage.data.get(), this->data.get(), rawImage.size);

    rawImage.format = this->format;

    auto module = Module::GetInstance<ImageModule>(Module::M_IMAGE);
    if (module == nullptr)
        throw love::Exception("love.image must be loaded in order to encode an ImageData.");

    for (auto* handler : module->GetFormatHandlers())
    {
        if (handler->CanEncode(this->format, encodedFormat))
        {
            encoder = handler;
            break;
        }
    }

    if (encoder)
    {
        std::unique_lock lock(this->mutex);
        image = encoder->Encode(rawImage, encodedFormat);
    }

    if (encoder == nullptr || image.data == nullptr)
    {
        const char* formatName = love::GetPixelFormatName(this->format);
        throw love::Exception("No suitable image encoder for the %s format.", formatName);
    }

    FileData* fileData = nullptr;

    try
    {
        fileData = new FileData(image.size, filename);
    }
    catch (love::Exception&)
    {
        throw;
    }

    std::memcpy(fileData->GetData(), image.data.get(), image.size);

    if (writeFile)
    {
        auto filesystem = Module::GetInstance<Filesystem>(Module::M_FILESYSTEM);

        if (filesystem == nullptr)
        {
            fileData->Release();
            throw love::Exception(
                "love.filesystem must be loaded in order to encode an ImageData.");
        }

        try
        {
            filesystem->Write(filename, fileData->GetData(), fileData->GetSize());
        }
        catch (love::Exception&)
        {
            fileData->Release();
            throw;
        }
    }

    return fileData;
}