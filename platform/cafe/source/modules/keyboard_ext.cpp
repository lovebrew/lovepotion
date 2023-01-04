#include <modules/keyboard_ext.hpp>

#include <utilities/result.hpp>

#include <codecvt>
#include <locale>
#include <string>

using namespace love;

Keyboard<Console::CAFE>::Keyboard() :
    Keyboard<>(this->GetMaxEncodingLength(MAX_INPUT_LENGTH)),
    createArgs {},
    appearArgs {},
    client(nullptr),
    inited(false),
    showing(false)
{}

void Keyboard<Console::CAFE>::Initialize()
{
    this->client = (FSClient*)MEMAllocFromDefaultHeap(sizeof(FSClient));

    if (!this->client)
        throw love::Exception("Failed to allocate FSClient for nn::swkbd!");

    const auto result = FSAddClient(this->client, FS_ERROR_FLAG_ALL);

    if (ResultCode(result).Failed())
        throw love::Exception("FSAddClient: %x", result);

    this->createArgs.regionType = nn::swkbd::RegionType::USA;
    this->createArgs.workMemory = MEMAllocFromDefaultHeap(nn::swkbd::GetWorkMemorySize(0));

    if (!this->createArgs.workMemory)
        throw love::Exception("No work memory for nn::swkbd!");

    this->createArgs.fsClient = this->client;

    if (!nn::swkbd::Create(this->createArgs))
        throw love::Exception("Failed to initialize nn:swkbd!");
}

Keyboard<Console::CAFE>::~Keyboard()
{
    FSDelClient(this->client, FS_ERROR_FLAG_ALL);
    MEMFreeToDefaultHeap(this->client);

    nn::swkbd::Destroy();
    MEMFreeToDefaultHeap(this->createArgs.workMemory);
}

void Keyboard<Console::CAFE>::Utf16toUtf8Text()
{
    const auto* str = nn::swkbd::GetInputFormString();
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> to_utf8;

    std::string converted = to_utf8.to_bytes(str);
    std::copy_n(converted.c_str(), converted.size(), this->text.get());
}

void Keyboard<Console::CAFE>::SetTextInput(const KeyboardOptions& options)
{
    uint32_t maxLength = this->GetMaxEncodingLength(options.maxLength);
    this->text         = std::make_unique<char[]>(maxLength);

    this->appearArgs.keyboardArg.configArg.languageType = nn::swkbd::LanguageType::English;

    /* utf8 to utf16le needed */
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> to_utf16;
    std::u16string hintText = to_utf16.from_bytes(options.hint.data());

    this->appearArgs.inputFormArg.hintText      = hintText.c_str();
    this->appearArgs.inputFormArg.maxTextLength = options.maxLength;
    this->appearArgs.inputFormArg.passwordMode =
        options.isPassword ? nn::swkbd::PasswordMode::Fade : nn::swkbd::PasswordMode::Clear;

    if (!nn::swkbd::AppearInputForm(this->appearArgs))
        return;

    this->showing = true;
}
