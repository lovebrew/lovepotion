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
    if (!this->open || (strncmp(this->mode, "w", 1) != 0))
    {
        Love::RaiseError("Cannot write to file %s. File not open for writing.", this->path);
        return;
    }

    fwrite(data, 1, length, this->fileHandle);
}

void File::Flush()
{
    fflush(this->fileHandle);
}

void File::Close()
{
    
    fclose(this->fileHandle);
}

char * File::Read()
{
    if (!this->open || (strncmp(this->mode, "r", 1) != 0))
    {
        Love::RaiseError("Cannot read file %s. File not open for reading.", this->path);
        return NULL;
    }

    char * buffer;

    long size = this->GetSize();

    buffer = (char *)malloc(size * sizeof(char));

    fread(buffer, 1, size, this->fileHandle);

    buffer[size] = '\0';

    return buffer;
}

u8 * File::ReadBinary()
{
    if (!this->open || (strncmp(this->mode, "rb", 2) != 0))
        return NULL;

    u8 * buffer;

    long size = this->GetSize();

    buffer = (u8 *)malloc(size * sizeof(u8));

    fread(buffer, 1, size, this->fileHandle);

    buffer[size] = '\0';

    return buffer;
}