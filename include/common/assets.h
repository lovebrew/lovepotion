/*
** common/assets.h
** @brief   : Loads game content
*/

class Assets
{
    public:
        Assets() = delete;
        static void Initialize(char * path);

    private:
        static inline std::string location = "romfs:/";

        static uint GetLocation(char * path);
};
