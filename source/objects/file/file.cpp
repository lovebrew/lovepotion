#include "common/runtime.h"
#include <sys/stat.h>

#include "objects/file/file.h"

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
    if (this->mode == MODE_WRITE && !this->Flush())
        return false;

    if (this->file == nullptr || (fclose(this->file) != 0))
        return false;

    this->mode = MODE_CLOSED;
    this->file = nullptr;

    return true;
}

bool File::Flush()
{
    if (!this->file || (this->mode != MODE_WRITE && mode != MODE_APPEND))
        throw love::Exception("File is not opened for writing.");

    return (fflush(this->file) == 0);
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

    return std::filesystem::file_size(this->GetFilename());
}

bool File::IsEOF()
{
    return (this->file == nullptr || feof(this->file));
}

bool File::IsOpen()
{
    return (this->mode != MODE_CLOSED && this->file != nullptr);
}

bool File::Open(File::Mode mode)
{
    if (mode == MODE_CLOSED)
        return true;

    if ((mode == MODE_READ) && !std::filesystem::exists(this->GetFilename()))
        throw love::Exception("Could not open file %s. Does not exist.", this->filename.c_str());

    if (this->file != nullptr)
        return false;

    FILE * handle = nullptr;

    switch (mode)
    {
        case MODE_APPEND:
            handle = fopen(this->GetFilename().c_str(), "a");
            break;
        case MODE_READ:
            handle = fopen(this->GetFilename().c_str(), "r");
            break;
        case MODE_WRITE:
            handle = fopen(this->GetFilename().c_str(), "w");
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

    int64_t read = 0;
    if (this->bufferMode == BUFFER_NONE)
        read = fread(destination, 1, size, this->file);
    else
        read = this->BufferedRead(destination, size);

    return read;
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
    LOG("Read: %lld Size: %lld", bytesRead, size)
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
    return (this->file != nullptr && fseek(this->file, position, SEEK_SET));
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

    // int ret = 1;
    // switch (mode)
    // {
    //     case BUFFER_NONE:
    //         ret =
    // }
    return true;
}

int64_t File::Tell()
{
    if (!this->file)
        return -1;

    return ftell(this->file);
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

    int64_t written = fwrite(data, 1, size, this->file);

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
