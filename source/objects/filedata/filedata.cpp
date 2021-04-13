#include "objects/filedata/filedata.h"

using namespace love;

love::Type FileData::type("FileData", &Data::type);

FileData::FileData(uint64_t size, const std::string& filename) :
    data(nullptr),
    size((size_t)size),
    filename(filename)
{
    try
    {
        this->data = new char[(size_t)size];
    }
    catch (std::bad_alloc&)
    {
        throw love::Exception("Out of memory.");
    }

    size_t extPos = filename.rfind('.');

    if (extPos != std::string::npos)
    {
        this->extension = filename.substr(extPos + 1);
        this->name      = filename.substr(0, extPos);
    }
    else
        this->name = filename;
}

FileData::FileData(const FileData& content) :
    data(nullptr),
    size(content.size),
    filename(content.filename),
    extension(content.extension),
    name(content.name)
{
    try
    {
        this->data = new char[(size_t)size];
    }
    catch (std::bad_alloc&)
    {
        throw love::Exception("Out of memory.");
    }

    memcpy(this->data, content.data, this->size);
}

FileData::~FileData()
{
    delete[] this->data;
}

FileData* FileData::Clone() const
{
    return new FileData(*this);
}

void* FileData::GetData() const
{
    return this->data;
}

size_t FileData::GetSize() const
{
    size_t max = std::numeric_limits<size_t>::max();
    return (this->size > max) ? max : (size_t)this->size;
}

const std::string& FileData::GetFilename() const
{
    return this->filename;
}

const std::string& FileData::GetExtension() const
{
    return this->extension;
}

const std::string& FileData::GetName() const
{
    return this->name;
}
