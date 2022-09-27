#include <common/exception.hpp>
#include <common/strongreference.hpp>

#include <objects/file/file.hpp>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <algorithm>

using namespace love;

Type File::type("File", &Stream::type);

FileData* File::Read()
{
    return this->Read(this->GetSize());
}

FileData* File::Read(int64_t size)
{
    bool isOpen = this->IsOpen();

    if (!isOpen && !this->Open(MODE_READ))
        throw love::Exception("Could not read file %s.", this->GetFilename().c_str());

    int64_t max     = this->GetSize();
    int64_t current = this->Tell();

    if (size < 0)
        throw love::Exception("Invalid read size.");

    current = std::clamp(current, (int64_t)0, max);

    if (current + size > max)
        size = max - current;

    StrongReference<FileData> fileData(new FileData(size, this->GetFilename()), Acquire::NORETAIN);
    int64_t bytesRead = this->Read(fileData->GetData(), size);

    if (bytesRead < 0 || (bytesRead == 0 && bytesRead != size))
    {
        delete fileData;
        throw love::Exception("Could not read from file.");
    }

    if (bytesRead < size)
    {
        StrongReference<FileData> temp(new FileData(bytesRead, this->GetFilename()),
                                       Acquire::NORETAIN);

        std::copy_n((const uint8_t*)fileData->GetData(), bytesRead, (uint8_t*)temp->GetData());
        fileData = temp;
    }

    if (!isOpen)
        this->Close();

    fileData->Retain();

    return fileData;
}

std::string File::GetExtension() const
{
    auto& filename   = this->GetFilename();
    const auto index = filename.rfind('.');

    if (index != std::string::npos)
        return filename.substr(index + 1);

    return std::string();
}

// clang-format off
constexpr BidirectionalMap modes = {
    "r", File::Mode::MODE_READ,
    "w", File::Mode::MODE_WRITE,
    "a", File::Mode::MODE_APPEND,
    "c", File::Mode::MODE_CLOSED
};

constexpr BidirectionalMap bufferModes = {
    "none", File::BufferMode::BUFFER_NONE,
    "line", File::BufferMode::BUFFER_LINE,
    "full", File::BufferMode::BUFFER_FULL
};
// clang-format on

bool File::GetConstant(const char* in, Mode& out)
{
    return modes.Find(in, out);
}

bool File::GetConstant(Mode in, const char*& out)
{
    return modes.ReverseFind(in, out);
}

SmallTrivialVector<const char*, 4> File::GetConstants(Mode mode)
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

SmallTrivialVector<const char*, 3> File::GetConstants(BufferMode mode)
{
    return bufferModes.GetNames();
}
