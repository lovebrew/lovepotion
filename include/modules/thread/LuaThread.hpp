#include "common/Data.hpp"
#include "common/Object.hpp"
#include "common/StrongRef.hpp"
#include "common/Variant.hpp"

#include "modules/thread/Threadable.hpp"

#include <vector>

namespace love
{
    class LuaThread : public Threadable
    {
      public:
        static Type type;

        LuaThread(const std::string& name, Data* code);

        virtual ~LuaThread()
        {}

        void run() override;

        const std::string& getError() const;

        bool hasError() const
        {
            return this->_hasError;
        }

        bool start(const std::vector<Variant>& args);

      private:
        void onError();

        StrongRef<Data> code;

        std::string name;
        std::string error;

        bool _hasError;

        std::vector<Variant> args;
    };
} // namespace love
