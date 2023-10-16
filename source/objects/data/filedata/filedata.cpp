#include <common/exception.hpp>
#include <objects/data/filedata/filedata.hpp>

#include <algorithm>
#include <limits>

#include <filesystem>

using namespace love;

Type FileData::type("FileData", &Data::type);

FileData::FileData(uint64_t size, const std::string& filename) :
    data(nullptr),
    size((size_t)size),
    filename(filename)
{
    try
    {
        this->data = std::make_unique<char[]>(this->size);
    }
    catch (std::bad_alloc&)
    {
        throw love::Exception("Out of memory.");
    }

    const auto path = std::filesystem::path(filename);

    if (path.has_extension())
    {
        this->extension = path.extension();
        this->name      = path.filename();
    }
    else
        this->name = path.filename();
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
        this->data = std::make_unique<char[]>(this->size);
    }
    catch (std::bad_alloc&)
    {
        throw love::Exception("Out of memory.");
    }

    std::copy_n(content.data.get(), this->size, this->data.get());
}

FileData* FileData::Clone() const
{
    return new FileData(*this);
}

void* FileData::GetData() const
{
    return this->data.get();
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
