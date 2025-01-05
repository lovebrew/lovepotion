#include "modules/filesystem/File.tcc"

namespace love
{
    FileData* FileBase::read(int64_t size)
    {
        bool isOpen = this->isOpen();

        if (!isOpen && !this->open(MODE_READ))
            throw love::Exception("Could not read file {}.", this->getFilename());

        int64_t max     = this->getSize();
        int64_t current = this->tell();

        if (size == SIZE_ALL)
            size = max;
        else if (size < 0)
            throw love::Exception("Invalid read size.");

        current = std::clamp(current, (int64_t)0, max);

        if (current + size > max)
            size = max - current;

        StrongRef<FileData> data(new FileData(size, this->getFilename()), Acquire::NO_RETAIN);
        int64_t bytesRead = this->read(data->getData(), size);

        if (bytesRead < 0 || (bytesRead == 0 && bytesRead != size))
        {
            delete data;
            throw love::Exception("Could not read from file");
        }

        if (bytesRead < size)
        {
            StrongRef<FileData> temp(new FileData(bytesRead, this->getFilename()), Acquire::NO_RETAIN);
            std::memcpy(temp->getData(), data->getData(), bytesRead);

            data = temp;
        }

        if (!isOpen)
            this->close();

        data->retain();
        return data;
    }
} // namespace love
