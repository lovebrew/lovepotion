#include "modules/filesystem/physfs/File.hpp"

#include "modules/filesystem/FileData.hpp"
#include "modules/filesystem/physfs/Filesystem.hpp"

#include <cstring>
#include <physfs.h>

namespace love
{
    static bool setupWriteDirectory()
    {
        auto fs = Module::getInstance<Filesystem>(Module::M_FILESYSTEM);
        return fs != nullptr && fs->setupWriteDirectory();
    }

    File::File(const std::string& filename, Mode mode) : FileBase(filename), file(nullptr)
    {
        if (!this->open(mode))
            throw love::Exception(E_COULD_NOT_OPEN_FILE, filename);
    }

    File::File(const File& other) : FileBase(other), file(nullptr)
    {
        if (!this->open(other.mode))
            throw love::Exception(E_COULD_NOT_OPEN_FILE, filename);
    }

    File::~File()
    {
        if (this->mode != MODE_CLOSED)
            this->close();
    }

    File* File::clone() const
    {
        return new File(*this);
    }

    bool File::open(Mode mode)
    {
        if (mode == MODE_CLOSED)
        {
            this->close();
            return true;
        }

        if (!PHYSFS_isInit())
            throw love::Exception(E_PHYSFS_NOT_INITIALIZED);

        if ((mode == MODE_READ) && !PHYSFS_exists(this->filename.c_str()))
            throw love::Exception(E_COULD_NOT_OPEN_FILE " Does not exist.", this->filename);

        if ((mode == MODE_APPEND || mode == MODE_WRITE) && !setupWriteDirectory())
            throw love::Exception("Could not set write directory.");

        if (this->file != nullptr)
            return false;

        PHYSFS_File* handle = nullptr;

        switch (mode)
        {
            case MODE_READ:
                handle = PHYSFS_openRead(this->filename.c_str());
                break;
            case MODE_APPEND:
                handle = PHYSFS_openAppend(this->filename.c_str());
                break;
            case MODE_WRITE:
                handle = PHYSFS_openWrite(this->filename.c_str());
                break;
            default:
                break;
        }

        if (handle == nullptr)
        {
            const char* error = Filesystem::getLastError();

            if (error == nullptr)
                error = "unknown error";

            throw love::Exception(E_PHYSFS_COULD_NOT_OPEN_FILE, this->filename, error);
        }

        this->file = handle;
        this->mode = mode;

        if (this->file != nullptr && !this->setBuffer(this->bufferMode, this->bufferSize))
        {
            this->bufferMode = BUFFER_NONE;
            this->bufferSize = 0;
        }

        return (this->file != nullptr);
    }

    bool File::close()
    {
        if (this->file == nullptr || !PHYSFS_close(this->file))
            return false;

        this->mode = MODE_CLOSED;
        this->file = nullptr;

        return true;
    }

    bool File::isOpen() const
    {
        return this->mode != MODE_CLOSED && this->file != nullptr;
    }

    int64_t File::getSize()
    {
        if (this->file == nullptr)
        {
            this->open(MODE_READ);
            int64_t size = PHYSFS_fileLength(this->file);
            this->close();

            return size;
        }

        return PHYSFS_fileLength(this->file);
    }

    int64_t File::tell()
    {
        if (this->file == nullptr)
            return -1;

        return PHYSFS_tell(this->file);
    }

    int64_t File::read(void* destination, int64_t size)
    {
        if (!this->file || this->mode != MODE_READ)
            throw love::Exception(E_FILE_NOT_OPEN_FOR_READING);

        if (size < 0)
            throw love::Exception(E_INVALID_READ_SIZE);

        return PHYSFS_readBytes(this->file, destination, (PHYSFS_uint64)size);
    }

    bool File::write(const void* data, int64_t size)
    {
        if (!this->file || (this->mode != MODE_WRITE && this->mode != MODE_APPEND))
            throw love::Exception(E_FILE_NOT_OPEN_FOR_WRITING);

        if (size < 0)
            throw love::Exception(E_INVALID_WRITE_SIZE);

        int64_t written = PHYSFS_writeBytes(this->file, data, (PHYSFS_uint64)size);

        if (written != size)
            return false;

        if (bufferMode == BUFFER_LINE && this->bufferSize > size)
        {
            if (std::memchr(data, '\n', (size_t)size) != nullptr)
                this->flush();
        }

        return true;
    }

    bool File::flush()
    {
        if (!this->file || (this->mode != MODE_WRITE && this->mode != MODE_APPEND))
            throw love::Exception(E_FILE_NOT_OPEN_FOR_WRITING);

        return PHYSFS_flush(this->file) != 0;
    }

    bool File::isEOF()
    {
        return this->file == nullptr || PHYSFS_eof(this->file);
    }

    bool File::seek(int64_t position, SeekOrigin origin)
    {
        if (this->file == nullptr)
            return false;

        if (origin == SEEKORIGIN_CURRENT)
            position += this->tell();
        else if (origin == SEEKORIGIN_END)
            position += this->getSize();

        if (position < 0)
            return false;

        return this->file != nullptr && PHYSFS_seek(this->file, (PHYSFS_uint64)position) != 0;
    }

    bool File::setBuffer(BufferMode mode, int64_t size)
    {
        if (size < 0)
            return false;

        if (!this->isOpen())
        {
            this->bufferMode = mode;
            this->bufferSize = size;
            return true;
        }

        int result = 1;

        switch (mode)
        {
            case BUFFER_NONE:
            default:
            {
                result = PHYSFS_setBuffer(this->file, 0);
                size   = 0;
                break;
            }
            case BUFFER_LINE:
            case BUFFER_FULL:
                result = PHYSFS_setBuffer(this->file, size);
                break;
        }

        if (result == 0)
            return false;

        this->bufferMode = mode;
        this->bufferSize = size;

        return true;
    }

    File::BufferMode File::getBuffer(int64_t& size) const
    {
        size = this->bufferSize;
        return this->bufferMode;
    }

    const std::string& File::getFilename() const
    {
        return this->filename;
    }
} // namespace love
