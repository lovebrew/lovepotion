#include "modules/keyboard/Keyboard.hpp"

#include "utf8.h"

using namespace nn::swkbd;

namespace love
{
    Keyboard::Keyboard() :
        KeyboardBase(),
        createArgs {},
        appearArgs {},
        client(nullptr),
        inited(false)
    {}

    Keyboard::~Keyboard()
    {
        if (!this->inited)
            return;

        FSDelClient(this->client, FS_ERROR_FLAG_ALL);
        MEMFreeToDefaultHeap(this->client);

        nn::swkbd::Destroy();
        MEMFreeToDefaultHeap(this->createArgs.workMemory);
    }

    void Keyboard::initialize()
    {
        this->client = (FSClient*)MEMAllocFromDefaultHeap(sizeof(FSClient));

        if (!this->client)
            throw love::Exception("Failed to allocate FSClient for nn::swkbd!");

        if (auto result = Result(FSAddClient(this->client, FS_ERROR_FLAG_ALL)); !result)
            throw love::Exception("FSAddClient: %x", result.get());

        this->createArgs.regionType = nn::swkbd::RegionType::USA;
        this->createArgs.workMemory = MEMAllocFromDefaultHeap(nn::swkbd::GetWorkMemorySize(0));
        this->appearArgs.keyboardArg.configArg.languageType = nn::swkbd::LanguageType::English;

        if (!this->createArgs.workMemory)
            throw love::Exception("No work memory for nn::swkbd!");

        this->createArgs.fsClient = this->client;

        if (!nn::swkbd::Create(this->createArgs))
            throw love::Exception("Failed to initialize nn:swkbd!");

        this->inited = true;
    }

    void Keyboard::setTextInput(const KeyboardOptions& options)
    {
        const auto length = this->getMaxEncodingLength(options.maxLength);

        try
        {
            this->text = std::make_unique<char[]>(length);
        }
        catch (std::bad_alloc&)
        {
            throw love::Exception(E_OUT_OF_MEMORY);
        }

        const auto hint = utf8::utf8to16(options.hint);

        this->appearArgs.inputFormArg.hintText      = hint.c_str();
        this->appearArgs.inputFormArg.maxTextLength = length;

        const auto password = options.password ? PasswordMode::Fade : PasswordMode::Clear;
        this->appearArgs.inputFormArg.passwordMode = password;

        this->showing = true;

        nn::swkbd::AppearInputForm(this->appearArgs);
    }
} // namespace love
