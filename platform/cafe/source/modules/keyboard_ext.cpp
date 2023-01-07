#include <modules/keyboard_ext.hpp>

#include <utilities/result.hpp>

#include <utf8.h>

#include <malloc.h>

using namespace love;

Keyboard<Console::CAFE>::Keyboard() :
    Keyboard<>(this->GetMaxEncodingLength(MAX_INPUT_LENGTH)),
    state(nullptr),
    createArgs {},
    appearArgs {},
    client(nullptr),
    inited(false)
{}

void Keyboard<Console::CAFE>::Initialize()
{
    this->state = (GX2ContextState*)memalign(GX2_CONTEXT_STATE_ALIGNMENT, sizeof(GX2ContextState));

    if (!this->state)
        throw love::Exception("Failed to allocate GX2ContextState for nn::swkbd!");

    GX2SetupContextStateEx(this->state, false);

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
    const auto* line = nn::swkbd::GetInputFormString();
    const auto utf8  = utf8::utf16to8(line);

    std::copy_n(utf8.c_str(), utf8.size(), this->text.get());
}

static nn::swkbd::PasswordMode GetPasswordMode(bool isPassword)
{
    if (isPassword)
        return nn::swkbd::PasswordMode::Fade;

    return nn::swkbd::PasswordMode::Clear;
}

void Keyboard<Console::CAFE>::SetTextInput(const KeyboardOptions& options)
{
    uint32_t maxLength = this->GetMaxEncodingLength(options.maxLength);
    this->text         = std::make_unique<char[]>(maxLength);

    this->appearArgs.keyboardArg.configArg.languageType = nn::swkbd::LanguageType::English;

    const auto hintText = utf8::utf8to16(options.hint);

    this->appearArgs.inputFormArg.hintText      = hintText.c_str();
    this->appearArgs.inputFormArg.maxTextLength = options.maxLength;
    this->appearArgs.inputFormArg.passwordMode  = GetPasswordMode(options.isPassword);

    nn::swkbd::AppearInputForm(this->appearArgs);
}
