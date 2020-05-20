#include "common/runtime.h"
#include <sys/stat.h>

#include "objects/file/file.h"
#include "modules/filesystem/wrap_filesystem.h"
#include <physfs.h>

using namespace love;

love::Type File::type("File", &Object::type);

File::File(const std::string & filename) : filename(filename),
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

File::BufferMode File::GetBuffer(int64_t & size) const
{
    size = this->bufferSize;

    return bufferMode;
}

const std::string & File::GetFilename() const
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
        this->Open(MODE_READ);

    int64_t size = (int64)PHYSFS_fileLength(file);

    this->Close();

    return size;
}

bool File::IsEOF()
{
    return PHYSFS_eof(file);
}

bool File::IsOpen()
{
    return (this->mode != MODE_CLOSED && this->file != nullptr);
}

bool File::Open(File::Mode mode)
{
    if (mode == MODE_CLOSED)
        return true;

    if ((mode == MODE_READ) && !PHYSFS_exists(this->filename.c_str()))
        throw love::Exception("Could not open file %s. Does not exist.", this->filename.c_str());

    if ((mode == MODE_APPEND || mode == MODE_WRITE) && (PHYSFS_getWriteDir() == nullptr) && !Wrap_Filesystem::SetupWriteDirectory())
        throw love::Exception("Could not set write directory.");

    if (this->file != nullptr)
        return false;

    PHYSFS_getLastErrorCode();
    PHYSFS_File * handle = nullptr;

    switch (mode)
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
        throw love::Exception("Could not open file %s.", this->GetFilename().c_str());

    this->file = handle;
    this->mode = mode;

    if (this->file != nullptr && !this->SetBuffer(this->bufferMode, this->bufferSize))
    {
        this->bufferMode = BUFFER_NONE;
        this->bufferSize = 0;
    }

    return (this->file != nullptr);
}

int64_t File::BufferedRead(void * destination, size_t size)
{
    // TO DO
    return 0;
}

int64_t File::Read(void * destination, int64_t size)
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

FileData * File::Read(int64_t size)
{
    if (!this->IsOpen() && !this->Open(MODE_READ))
        throw love::Exception("Could not read file %s.", this->GetFilename().c_str());

    int64_t max = this->GetSize();
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

    FileData * fileData = new FileData(size, this->GetFilename());

    int64_t bytesRead = this->Read(fileData->GetData(), size);

    if (bytesRead < 0 || (bytesRead == 0 && bytesRead != size))
    {
        delete fileData;
        throw love::Exception("Could not read from file.");
    }

    if (bytesRead < size)
    {
        FileData * tmp = new FileData(bytesRead, this->GetFilename());
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
            ret = PHYSFS_setBuffer(this->file, 0);
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

    return (int64)PHYSFS_tell(file);
}

bool File::Write(Data * data, int64_t size)
{
    return this->Write(data->GetData(), (size == ALL) ? data->GetSize() : size);
}

bool File::Write(const void * data, int64_t size)
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

bool File::GetConstant(const char * in, Mode & out)
{
    return modes.Find(in, out);
}

bool File::GetConstant(Mode in, const char *& out)
{
    return modes.Find(in, out);
}

std::vector<std::string> File::GetConstants(Mode mode)
{
    return modes.GetNames();
}

StringMap<File::Mode, File::MODE_MAX_ENUM>::Entry File::modeEntries[] = {
    { "c", MODE_CLOSED },
    { "r", MODE_READ   },
    { "w", MODE_WRITE  },
    { "a", MODE_APPEND }
};

StringMap<File::Mode, File::MODE_MAX_ENUM> File::modes(File::modeEntries, sizeof(File::modeEntries));

/* BUFFER MODES */

bool File::GetConstant(const char * in, BufferMode & out)
{
    return bufferModes.Find(in, out);
}

bool File::GetConstant(BufferMode in, const char *& out)
{
    return bufferModes.Find(in, out);
}

std::vector<std::string> File::GetConstants(BufferMode mode)
{
    return bufferModes.GetNames();
}

StringMap<File::BufferMode, File::BUFFER_MAX_ENUM>::Entry File::bufferModeEntries[] = {
    { "none", BUFFER_NONE },
    { "line", BUFFER_LINE },
    { "full", BUFFER_FULL }
};

StringMap<File::BufferMode, File::BUFFER_MAX_ENUM> File::bufferModes(File::bufferModeEntries, sizeof(File::bufferModeEntries));
