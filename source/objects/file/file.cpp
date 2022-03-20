#include "objects/file/file.h"
#include "common/bidirectionalmap.h"
#include <sys/stat.h>

using namespace love;

extern bool SetupWriteDirectory();

love::Type File::type("File", &Object::type);

File::File(const std::string& filename) :
    filename(filename),
    file(nullptr),
    mode(MODE_CLOSED),
    bufferMode(BUFFER_NONE),
    bufferSize(0)
{}

File::~File()
{
    if (this->mode != MODE_CLOSED)
        this->Close();
}

bool File::Close()
{
    if (this->file == nullptr || !PHYSFS_close(file))
        return false;

    this->mode = MODE_CLOSED;
    this->file = nullptr;

    return true;
}

bool File::Flush()
{
    if (!this->file || (this->mode != MODE_WRITE && mode != MODE_APPEND))
        throw love::Exception("File is not opened for writing.");

    return PHYSFS_flush(this->file) != 0;
}

File::BufferMode File::GetBuffer(int64_t& size) const
{
    size = this->bufferSize;

    return bufferMode;
}

const std::string& File::GetFilename() const
{
    return this->filename;
}

File::Mode File::GetMode()
{
    return this->mode;
}

int64_t File::GetSize()
{
    if (this->file == nullptr)
    {
        this->Open(MODE_READ);
        int64_t size = (int64_t)PHYSFS_fileLength(file);
        this->Close();
        return size;
    }

    return (int64_t)PHYSFS_fileLength(file);
}

bool File::IsEOF()
{
    return PHYSFS_eof(file);
}

bool File::IsOpen()
{
    return (this->mode != MODE_CLOSED && this->file != nullptr);
}

bool File::Open(File::Mode openMode)
{
    if (openMode == MODE_CLOSED)
        return true;

    if (!PHYSFS_isInit())
        throw love::Exception("PhysFS is not initialized.");

    if ((openMode == MODE_READ) && !PHYSFS_exists(this->filename.c_str()))
        throw love::Exception("Could not open file %s. Does not exist.", this->filename.c_str());

    if ((openMode == MODE_APPEND || openMode == MODE_WRITE) && (PHYSFS_getWriteDir() == nullptr) &&
        !SetupWriteDirectory())
        throw love::Exception("Could not set write directory.");

    if (this->file != nullptr)
        return false;

    PHYSFS_getLastErrorCode();
    PHYSFS_File* handle = nullptr;

    switch (openMode)
    {
        case MODE_APPEND:
            handle = PHYSFS_openAppend(this->filename.c_str());
            break;
        case MODE_READ:
            handle = PHYSFS_openRead(this->filename.c_str());
            break;
        case MODE_WRITE:
            handle = PHYSFS_openWrite(this->filename.c_str());
            break;
        default:
            break;
    }

    if (handle == nullptr)
    {
        const char* error = PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());

        if (error == nullptr)
            error = "unknown error";

        throw love::Exception("Could not open file %s (%s)", this->filename.c_str(), error);
    }

    this->file = handle;
    this->mode = openMode;

    if (this->file != nullptr && !this->SetBuffer(this->bufferMode, this->bufferSize))
    {
        this->bufferMode = BUFFER_NONE;
        this->bufferSize = 0;
    }

    return (this->file != nullptr);
}

int64_t File::Read(void* destination, int64_t size)
{
    if (!this->file || this->mode != MODE_READ)
        throw love::Exception("File is not opened for reading.");

    long selfSize = this->GetSize();

    size = (size == ALL) ? selfSize : size;
    size = (size > selfSize) ? selfSize : size;

    if (size < 0)
        throw love::Exception("Invalid read size.");

    return PHYSFS_readBytes(this->file, destination, (PHYSFS_uint64)size);
}

FileData* File::Read(int64_t size)
{
    if (!this->IsOpen() && !this->Open(MODE_READ))
        throw love::Exception("Could not read file %s.", this->GetFilename().c_str());

    int64_t max     = this->GetSize();
    int64_t current = this->Tell();

    size = (size == ALL) ? max : size;

    if (size < 0)
        throw love::Exception("Invalid read size.");

    if (current < 0)
        current = 0;
    else if (current > max)
        current = max;

    if (current + size > max)
        size = max - current;

    FileData* fileData = new FileData(size, this->GetFilename());

    int64_t bytesRead = this->Read(fileData->GetData(), size);

    if (bytesRead < 0 || (bytesRead == 0 && bytesRead != size))
    {
        delete fileData;
        throw love::Exception("Could not read from file.");
    }

    if (bytesRead < size)
    {
        FileData* tmp = new FileData(bytesRead, this->GetFilename());
        memcpy(tmp->GetData(), fileData->GetData(), (size_t)bytesRead);

        fileData->Release();
        fileData = tmp;
    }

    if (!this->IsOpen())
        this->Close();

    return fileData;
}

bool File::Seek(u_int64_t position)
{
    return this->file != nullptr && PHYSFS_seek(this->file, (PHYSFS_uint64)position) != 0;
}

bool File::SetBuffer(BufferMode mode, int64_t size)
{
    if (size < 0)
        return false;

    if (!this->IsOpen())
    {
        this->bufferMode = mode;
        this->bufferSize = size;

        return true;
    }

    int ret = 1;
    switch (mode)
    {
        case BUFFER_NONE:
        default:
            ret  = PHYSFS_setBuffer(this->file, 0);
            size = 0;
            break;
        case BUFFER_LINE:
        case BUFFER_FULL:
            ret = PHYSFS_setBuffer(this->file, size);
            break;
    }

    if (ret == 0)
        return false;

    this->bufferMode = mode;
    this->bufferSize = size;

    return true;
}

int64_t File::Tell()
{
    if (!this->file)
        return -1;

    return (int64_t)PHYSFS_tell(file);
}

bool File::Write(Data* data, int64_t size)
{
    return this->Write(data->GetData(), (size == ALL) ? data->GetSize() : size);
}

bool File::Write(const void* data, int64_t size)
{
    if (!this->file || (this->mode != MODE_WRITE && this->mode != MODE_APPEND))
        throw love::Exception("File is not opened for writing.");

    if (size < 0)
        throw love::Exception("Invalid write size.");

    int64_t written = PHYSFS_writeBytes(this->file, data, (PHYSFS_uint64)size);

    if (written != size)
        return false;

    // manually flush buffer
    if (this->bufferMode == BUFFER_LINE && this->bufferSize > size)
    {
        if (memchr(data, '\n', (size_t)size) != nullptr)
            this->Flush();
    }

    return true;
}

/* OPEN MODES */

// clang-format off
constexpr auto modes = BidirectionalMap<>::Create(
    "r", File::Mode::MODE_READ,
    "w", File::Mode::MODE_WRITE,
    "a", File::Mode::MODE_APPEND,
    "c", File::Mode::MODE_CLOSED
);

constexpr auto bufferModes = BidirectionalMap<>::Create(
    "none", File::BufferMode::BUFFER_NONE,
    "line", File::BufferMode::BUFFER_LINE,
    "full", File::BufferMode::BUFFER_FULL
);
// clang-format on

bool File::GetConstant(const char* in, Mode& out)
{
    return modes.Find(in, out);
}

bool File::GetConstant(Mode in, const char*& out)
{
    return modes.ReverseFind(in, out);
}

std::vector<const char*> File::GetConstants(Mode mode)
{
    return modes.GetNames();
}

bool File::GetConstant(const char* in, BufferMode& out)
{
    return bufferModes.Find(in, out);
}

bool File::GetConstant(BufferMode in, const char*& out)
{
    return bufferModes.ReverseFind(in, out);
}

std::vector<const char*> File::GetConstants(BufferMode mode)
{
    return bufferModes.GetNames();
}
