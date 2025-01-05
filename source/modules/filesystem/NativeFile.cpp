#include "modules/filesystem/NativeFile.hpp"

#include <sys/stat.h>
#include <sys/types.h>

#include <stdio.h>
#include <unistd.h>

namespace love
{
    NativeFile::NativeFile(const std::string& filename, Mode mode) : FileBase(filename), file(nullptr)
    {
        if (!this->open(mode))
            throw love::Exception(E_COULD_NOT_OPEN_FILE, filename);
    }

    NativeFile::NativeFile(const NativeFile& other) : FileBase(other), file(nullptr)
    {
        if (!this->open(mode))
            throw love::Exception(E_COULD_NOT_OPEN_FILE, filename);
    }

    NativeFile::~NativeFile()
    {
        if (this->mode != MODE_CLOSED)
            this->close();
    }

    NativeFile* NativeFile::clone() const
    {
        return new NativeFile(*this);
    }

    bool NativeFile::open(Mode mode)
    {
        if (mode == MODE_CLOSED)
        {
            this->close();
            return true;
        }

        if (this->file != nullptr)
            return false;

        if (mode == MODE_READ && this->file == nullptr)
            throw love::Exception(E_COULD_NOT_OPEN_FILE, filename);

        this->mode = mode;

        if (this->file != nullptr && !this->setBuffer(this->bufferMode, this->bufferSize))
        {
            this->bufferMode = BUFFER_NONE;
            this->bufferSize = 0;
        }

        return this->file != nullptr;
    }

    bool NativeFile::close()
    {
        if (this->file == nullptr || std::fclose(this->file) != 0)
            return false;

        this->mode = MODE_CLOSED;
        this->file = nullptr;

        return true;
    }

    bool NativeFile::isOpen() const
    {
        return this->mode != MODE_CLOSED && this->file != nullptr;
    }

    int64_t NativeFile::getSize()
    {
        int fd = this->file ? fileno(this->file) : -1;

        struct stat buffer;
        std::memset(&buffer, 0, sizeof(buffer));

        if (fd != -1)
        {
            if (fstat(fd, &buffer) != 0)
                return -1;
        }
        else if (stat(filename.c_str(), &buffer) != 0)
            return -1;

        return (int64_t)buffer.st_size;
    }

    int64_t NativeFile::read(void* destination, int64_t size)
    {
        if (!this->file || this->mode != MODE_READ)
            throw love::Exception(E_FILE_NOT_OPEN_FOR_READING);

        if (size < 0)
            throw love::Exception(E_INVALID_READ_SIZE);

        const auto read = std::fread(destination, 1, (size_t)size, this->file);
        return (int64_t)read;
    }

    bool NativeFile::write(const void* data, int64_t size)
    {
        if (!this->file || (this->mode != MODE_WRITE && this->mode != MODE_APPEND))
            throw love::Exception(E_FILE_NOT_OPEN_FOR_WRITING);

        if (size < 0)
            throw love::Exception(E_INVALID_WRITE_SIZE);

        const auto written = (int64_t)std::fwrite(data, 1, (size_t)size, this->file);
        return written == size;
    }

    bool NativeFile::flush()
    {
        if (!this->file || (this->mode != MODE_WRITE && this->mode != MODE_APPEND))
            throw love::Exception(E_FILE_NOT_OPEN_FOR_WRITING);

        return std::fflush(this->file) == 0;
    }

    bool NativeFile::isEOF()
    {
        return this->file == nullptr || this->tell() >= this->getSize();
    }

    int64_t NativeFile::tell()
    {
        if (this->file == nullptr)
            return -1;

        return (int64_t)ftello(this->file);
    }

    bool NativeFile::seek(int64_t position, SeekOrigin origin)
    {
        if (this->file == nullptr)
            return false;

        int fOrigin = SEEK_SET;
        if (origin == SEEKORIGIN_CURRENT)
            fOrigin = SEEK_CUR;
        else if (origin == SEEKORIGIN_END)
            fOrigin = SEEK_END;

        return fseeko(this->file, (off_t)position, fOrigin) == 0;
    }

    bool NativeFile::setBuffer(BufferMode bufferMode, int64_t size)
    {
        if (size < 0)
            return false;

        if (bufferMode == BUFFER_NONE)
            size = 0;

        if (!this->isOpen())
        {
            this->bufferMode = bufferMode;
            this->bufferSize = size;
            return true;
        }

        int vBufMode;
        switch (bufferMode)
        {
            case FileBase::BUFFER_NONE:
            default:
            {
                vBufMode = _IONBF;
                break;
            }
            case FileBase::BUFFER_LINE:
            {
                vBufMode = _IOLBF;
                break;
            }
            case FileBase::BUFFER_FULL:
            {
                vBufMode = _IOFBF;
                break;
            }
        }

        if (setvbuf(this->file, nullptr, vBufMode, (size_t)size) != 0)
            return false;

        this->bufferMode = bufferMode;
        this->bufferSize = bufferSize;
        return true;
    }

    FileBase::BufferMode NativeFile::getBuffer(int64_t& size) const
    {
        size = this->bufferSize;
        return this->bufferMode;
    }

    const std::string& NativeFile::getFilename() const
    {
        return this->filename;
    }

    const char* NativeFile::getModeString(Mode mode)
    {
        switch (mode)
        {
            case FileBase::MODE_CLOSED:
            default:
                return "c";
            case FileBase::MODE_READ:
                return "rb";
            case FileBase::MODE_WRITE:
                return "wb";
            case FileBase::MODE_APPEND:
                return "ab";
        }
    }
} // namespace love
