#pragma once

namespace love
{
    namespace version
    {
        class Version
        {
          public:
            consteval Version(std::string_view version) : values()
            {
                int position = 0;
                size_t last  = 0;
                size_t size  = 0;

                for (size_t index = 0; index < version.length(); index++)
                {
                    if (version.at(index) == DELIMETER)
                    {
                        std::string_view token = version.substr(last, size);
                        this->values[position] = Version::atoi(token);

                        last = index + 1;
                        position++;
                        size = 0;
                    }
                    else
                    {
                        if (position == 2)
                        {
                            std::string_view token = version.substr(last, version.length() - last);
                            this->values[position++] = Version::atoi(token);
                        }
                        size++;
                    }
                }
            }

            consteval int Major() const
            {
                return this->values[0];
            }

            consteval int Minor() const
            {
                return this->values[1];
            }

            consteval int Revision() const
            {
                return this->values[2];
            }

          private:
            static constexpr char DELIMETER = '.';

            constexpr static int atoi(std::string_view view)
            {
                int value = 0;
                for (const auto& character : view)
                {
                    if ('0' <= character && character <= '9')
                    {
                        value = value * 10 + character - '0';
                    }
                }
                return value;
            }

            std::array<int, 3> values;
        };

        static constexpr Version LOVE_POTION(__APP_VERSION__);
        static constexpr Version LOVE_FRAMEWORK(__LOVE_VERSION__);

        static constexpr const char* CODENAME        = "Mysterious Mysteries";
        static constexpr const char* COMPATABILITY[] = { __APP_VERSION__, "2.2.0", 0 };
    } // namespace version
} // namespace love
