#pragma once
#include <cstdint>
#include "Log.h"
#include "../server/ShmServerInclude.h"
#include "../server/Connection.h"
#include "../server/ServerConfiguration.h"
#include "../server/Listener.h"

namespace shm::Core
{
    class Application
    {
    public:
        Application() = default;

        static auto Get() -> Application &;

        auto Start() const -> int32_t;
    };
}
