#include <common/exception.hpp>

#include <modules/filesystem/physfs/filesystem.hpp>
#include <modules/filesystem/wrap_filesystem.hpp>

#include <objects/file/physfs/file.hpp>

using namespace love::physfs;

File::File(const std::string& filename, Mode mode) :
    filename(filename),
    file(nullptr),
    fileMode(MODE_CLOSED),
    bufferMode(BUFFER_NONE),
    bufferSize(0)
{
    if (!this->Open(mode))
        throw love::Exception("Could not open file at path %s.", filename.c_str());
}

File::File(const File& other) :
    filename(other.filename),
    file(nullptr),
    fileMode(MODE_CLOSED),
    bufferMode(other.bufferMode),
    bufferSize(other.bufferSize)
{
    if (!this->Open(other.fileMode))
        throw love::Exception("Could not open file at path %s", filename.c_str());
}

File::~File()
{
    if (this->fileMode != MODE_CLOSED)
        this->Close();
}

File* File::Clone()
{
    return new File(*this);
}

bool File::Open(Mode mode)
{
    if (mode == MODE_CLOSED)
    {
        this->Close();
        return true;
    }

    if (!PHYSFS_isInit())
        throw love::Exception("PhysFS is not initialized.");

    if ((mode == MODE_READ) && !PHYSFS_exists(this->filename.c_str()))
        throw love::Exception("Could not open file %s. Does not exist.", this->filename.c_str());

    if ((mode == MODE_APPEND || mode == MODE_WRITE) && !Wrap_Filesystem::SetupWriteDirectory())
        throw love::Exception("Could not set write directory.");

    if (this->file != nullptr)
        return false;

    PHYSFS_File* handle = nullptr;

    switch (mode)
    {
        case MODE_READ:
        {
            handle = PHYSFS_openRead(this->filename.c_str());
            break;
        }
        case MODE_APPEND:
        {
            handle = PHYSFS_openAppend(this->filename.c_str());
            break;
        }
        case MODE_WRITE:
        {
            handle = PHYSFS_openWrite(this->filename.c_str());
            break;
        }
        default:
            break;
    }

    if (!handle)
    {
        const char* error = Filesystem::GetLastError();

        if (error == nullptr)
            error = "unknown error";

        throw love::Exception("Could not open file %s (errno %d: %s).", filename.c_str(), errno,
                              error);
    }

    this->file     = handle;
    this->fileMode = mode;

    if (this->file != nullptr && this->SetBuffer(this->bufferMode, this->bufferSize))
    {
        this->bufferMode = BUFFER_NONE;
        this->bufferSize = 0;
    }

    return (this->file != nullptr);
}

bool File::Close()
{
    if (!this->file || !PHYSFS_close(this->file))
        return false;

    this->fileMode = MODE_CLOSED;
    this->file     = nullptr;

    return true;
}

bool File::IsOpen() const
{
    return (this->fileMode != MODE_CLOSED) && (this->file != nullptr);
}

int64_t File::GetSize()
{
    if (!this->file)
    {
        this->Open(MODE_READ);
        auto size = PHYSFS_fileLength(this->file);
        this->Close();

        return size;
    }

    return PHYSFS_fileLength(this->file);
}

int64_t File::Read(void* destination, int64_t size)
{
    if (!this->file || this->fileMode != MODE_READ)
        throw love::Exception("File is not opened for reading.");

    if (size < 0)
        throw love::Exception("Invalid read size: %lld", size);

    return PHYSFS_readBytes(this->file, destination, size);
}

bool File::Write(const void* data, int64_t size)
{
    if (!this->file || (this->fileMode != MODE_WRITE && this->fileMode != MODE_APPEND))
        throw love::Exception("File is not opened for writing.");

    if (size < 0)
        throw love::Exception("Invalid write size: %lld", size);

    int64_t written = PHYSFS_writeBytes(this->file, data, size);

    if (written != size)
        return false;

    if (this->bufferMode == BUFFER_LINE && this->bufferSize > size)
    {
        if (memchr(data, '\n', size) != nullptr)
            this->Flush();
    }

    return true;
}

bool File::Flush()
{
    if (!this->file || (this->fileMode != MODE_WRITE && this->fileMode != MODE_APPEND))
        throw love::Exception("File is not opened for writing.");

    return (PHYSFS_flush(this->file) != 0);
}

bool File::IsEOF()
{
    return ((this->file == nullptr) || PHYSFS_eof(this->file));
}

int64_t File::Tell()
{
    if (!this->file)
        return -1;

    return PHYSFS_tell(this->file);
}

bool File::Seek(int64_t position, SeekOrigin origin)
{
    if (this->file != nullptr)
    {
        if (origin == ORIGIN_CURRENT)
            position += this->Tell();
        else if (origin == ORIGIN_END)
            position += this->GetSize();
    }

    if (position < 0)
        return false;

    return ((this->file != nullptr) && (PHYSFS_seek(this->file, position) != 0));
}

bool File::SetBuffer(BufferMode bufferMode, int64_t size)
{
    if (size < 0)
        return false;

    if (!this->IsOpen())
    {
        this->bufferMode = bufferMode;
        this->bufferSize = size;

        return true;
    }

    int result = 1;

    switch (bufferMode)
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
        {
            result = PHYSFS_setBuffer(this->file, size);
            break;
        }
    }

    if (result == 0)
        return false;

    this->bufferMode = bufferMode;
    this->bufferSize = size;

    return true;
}

File::BufferMode File::GetBuffer(int64_t& size) const
{
    size = this->bufferSize;

    return this->bufferMode;
}

const std::string& File::GetFilename() const
{
    return this->filename;
}

love::File::Mode File::GetMode() const
{
    return this->fileMode;
}
