#include <modules/keyboard_ext.hpp>

#include <utilities/result.hpp>

#include <codecvt>
#include <locale>
#include <string>

using namespace love;

Keyboard<Console::CAFE>::Keyboard() :
    Keyboard<>(this->GetMaxEncodingLength(MAX_INPUT_LENGTH)),
    createArgs {},
    appearArgs {}
{
    this->client = (FSClient*)MEMAllocFromDefaultHeap(sizeof(FSClient));
    FSAddClient(this->client, FS_ERROR_FLAG_NONE);

    this->thread = new SwkbdThread();
}

Keyboard<Console::CAFE>::~Keyboard()
{
    FSDelClient(this->client, FS_ERROR_FLAG_NONE);
    MEMFreeToDefaultHeap(this->client);

    this->thread->SetFinish();
    this->thread->Wait();

    delete this->thread;
}

std::string Keyboard<Console::CAFE>::SetTextInput(const KeyboardOptions& options)
{
    uint32_t maxLength = this->GetMaxEncodingLength(options.maxLength);
    this->text         = std::make_unique<char[]>(maxLength);

    this->createArgs.regionType = nn::swkbd::RegionType::USA;
    this->createArgs.workMemory = MEMAllocFromDefaultHeap(nn::swkbd::GetWorkMemorySize(0));
    this->createArgs.fsClient   = this->client;

    R_UNLESS(nn::swkbd::Create(this->createArgs) ? 0 : -1, std::string {});

    nn::swkbd::MuteAllSound(false);

    this->appearArgs.keyboardArg.configArg.languageType = nn::swkbd::LanguageType::English;

    /* utf8 to utf16le needed */
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> to_utf16;
    std::u16string hintText = to_utf16.from_bytes(options.hint.data());

    this->appearArgs.inputFormArg.hintText      = hintText.c_str();
    this->appearArgs.inputFormArg.maxTextLength = options.maxLength;
    this->appearArgs.inputFormArg.passwordMode =
        options.isPassword ? nn::swkbd::PasswordMode::Fade : nn::swkbd::PasswordMode::Clear;

    R_UNLESS(nn::swkbd::AppearInputForm(this->appearArgs) ? 0 : -1, std::string {});

    this->thread->Start();
    while (this->thread->IsRunning())
    {}

    nn::swkbd::Destroy();
    MEMFreeToDefaultHeap(this->createArgs.workMemory);

    /* utf16le to utf8 needed */
    const auto* str = nn::swkbd::GetInputFormString();

    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> to_utf8;

    return to_utf8.to_bytes(str);
}
