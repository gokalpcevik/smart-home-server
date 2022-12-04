#pragma once
#include <cstdint>
#include "Log.h"
#include "../server/ShmServerInclude.h"
#include "../server/Connection.h"
#include "../server/ServerConfiguration.h"

namespace shm::Core
{
    class Application
    {
    public:
        Application() = default;

        static auto Get() -> Application &;

        auto Start() -> int32_t;
    private:
        auto Update() ->int32_t;
        void Serve(tcp::acceptor& acceptor, tcp::socket& socket);

    private:
        boost::asio::ip::address m_IpAddress{};
        uint16_t m_Port{};
        net::io_context m_IOContext{1};

    };
}
