#include "common/Exception.hpp"

#include "modules/image/CompressedImageData.hpp"

#define E_COULD_NOT_PARSE_COMPRESSED_IMAGE_DATA "Could not parse compressed data: {:s}"

namespace love
{
    Type CompressedImageData::type("CompressedImageData", &Data::type);

    CompressedImageData::CompressedImageData(const std::list<FormatHandler*>& formats, Data* fileData) :
        format(PIXELFORMAT_UNKNOWN)
    {
        FormatHandler* parser = nullptr;

        for (auto* handler : formats)
        {
            if (handler->canParseCompressed(fileData))
            {
                parser = handler;
                break;
            }
        }

        if (parser == nullptr)
            throw love::Exception(E_COULD_NOT_PARSE_COMPRESSED_IMAGE_DATA, "Unknown format.");

        memory = parser->parseCompressed(fileData, this->dataImages, this->format);

        if (memory == nullptr)
            throw love::Exception(E_COULD_NOT_PARSE_COMPRESSED_IMAGE_DATA, "Memory allocation failed.");

        if (this->dataImages.size() == 0 || memory->getSize() == 0)
            throw love::Exception(E_COULD_NOT_PARSE_COMPRESSED_IMAGE_DATA, "No valid data?");

        this->format = love::getLinearPixelFormat(this->format);
    }

    CompressedImageData::CompressedImageData(const CompressedImageData& other) : format(other.format)
    {
        this->memory.set(other.memory->clone(), Acquire::NO_RETAIN);

        // clang-format off
        for (const auto& image : other.dataImages)
        {
            auto* slice = new CompressedSlice(image->getFormat(), image->getWidth(), image->getHeight(), memory, image->getOffset(), image->getSize());
            this->dataImages.push_back(slice);
            slice->release();
        }
        // clang-format on
    }

    CompressedImageData::~CompressedImageData()
    {}

    CompressedImageData* CompressedImageData::clone() const
    {
        return new CompressedImageData(*this);
    }

    size_t CompressedImageData::getSize() const
    {
        return this->memory->getSize();
    }

    void* CompressedImageData::getData() const
    {
        return this->memory->getData();
    }

    int CompressedImageData::getMipmapCount() const
    {
        return this->dataImages.size();
    }

    int CompressedImageData::getSliceCount(int) const
    {
        return 1;
    }

    size_t CompressedImageData::getSize(int mipmap) const
    {
        this->checkSliceExists(0, mipmap);
        return this->dataImages[mipmap]->getSize();
    }

    void* CompressedImageData::getData(int mipmap) const
    {
        this->checkSliceExists(0, mipmap);
        return this->dataImages[mipmap]->getData();
    }

    int CompressedImageData::getWidth(int mipmap) const
    {
        this->checkSliceExists(0, mipmap);
        return this->dataImages[mipmap]->getWidth();
    }

    int CompressedImageData::getHeight(int mipmap) const
    {
        this->checkSliceExists(0, mipmap);
        return this->dataImages[mipmap]->getHeight();
    }

    PixelFormat CompressedImageData::getFormat() const
    {
        return this->format;
    }

    bool CompressedImageData::isLinear() const
    {
        return this->dataImages.empty() ? false : this->dataImages[0]->isLinear();
    }

    void CompressedImageData::setLinear(bool linear)
    {
        for (auto& slice : this->dataImages)
            slice->setLinear(linear);
    }

    CompressedSlice* CompressedImageData::getSlice(int slice, int mipmap) const
    {
        this->checkSliceExists(slice, mipmap);
        return this->dataImages[mipmap].get();
    }

    void CompressedImageData::checkSliceExists(int slice, int mipmap) const
    {
        if (slice != 0)
            throw love::Exception("Slice index {:d} does not exist.", slice + 1);

        if (mipmap < 0 || mipmap >= (int)this->dataImages.size())
            throw love::Exception("Mipmap level {:d} does not exist.", mipmap + 1);
    }
} // namespace love
