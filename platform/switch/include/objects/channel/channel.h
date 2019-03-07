#pragma once 

class Channel : public Object
{
    public:
        Channel();
        Channel(const std::string & name);

        void Push(const Variant & variant);
        bool Pop(Variant * variant);

    private:
        std::queue<Variant> content;
        std::string name;
        Mutex mutex;
};

extern std::map<std::string, Channel *> channels;