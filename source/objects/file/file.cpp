#include "common/runtime.h"
#include <sys/stat.h>

#include "objects/file/file.h"

File::File(const char * path) : Object("File")
{
    this->path = strdup(path);
    this->open = false;
}

File::File(const char * path, const char * mode) : Object("File")
{
    this->path = strdup(path);
    this->open = false;

    this->Open(mode);
}

File::~File()
{
    if (this->open)
        this->Flush();

    this->Close();
}

long File::GetSize()
{
    long size = 0;
    
    fseek(this->fileHandle, 0, SEEK_END);
    size = ftell(this->fileHandle);
    rewind(this->fileHandle);

    return size;
}

const char * File::GetMode()
{
    return this->mode;
}

bool File::IsOpen()
{
    return this->open;
}

bool File::Open(const char * mode)
{
    this->fileHandle = fopen(this->path, mode);

    if (!this->fileHandle)
    {
        fclose(this->fileHandle);
        return false;
    }

    this->mode = strdup(mode);
    this->open = true;

    return true;
}

void File::Write(const char * data, size_t length)
{
    if (this->open && (strncmp(this->mode, "w", 1) == 0))
        fwrite(data, 1, length, this->fileHandle);
    else
        Love::RaiseError("Cannot write to file %s. File not open for writing.", this->path);
}

void File::Flush()
{
    fflush(this->fileHandle);
}

void File::Close()
{
    
    fclose(this->fileHandle);
}

char *File::Read(size_t &size)
{
    if (!this->open || (strncmp(this->mode, "r", 1) != 0))
    {
        Love::RaiseError("Cannot read file %s. File not open for reading.", this->path);
        return 0;
    }

    size = this->GetSize();

    char *buffer = (char *)malloc(size * sizeof(char));
    if (!buffer) {
      Love::RaiseError("Out of memory");
      return 0;
    }

    fread(buffer, 1, size, this->fileHandle);
    return buffer;
}

u8 * File::ReadBinary(size_t &size)
{
    if (!this->open || (strncmp(this->mode, "rb", 2) != 0))
        return 0;

    size = this->GetSize();

    u8 * buffer = (u8 *)malloc(size * sizeof(u8));
    if (!buffer) {
      Love::RaiseError("Out of memory");
      return 0;
    }

    fread(buffer, 1, size, this->fileHandle);

    return buffer;
}