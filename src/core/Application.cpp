#include "Application.h"


namespace shm::Core
{
    auto Application::Get() -> Application &
    {
        static Application instance;
        return instance;
    }

    auto Application::Start() const -> int32_t
    {
        Log::Init();

        shm_info("Server is now listening on port {0}, running on {1} threads.", config::g_Port, config::g_Threads);
        boost::asio::ip::address const address = net::ip::make_address(config::g_IpAddress);
        net::io_context ioc{ config::g_Threads };
        auto const listener = std::make_shared<server::Listener>(ioc, tcp::endpoint{ address, config::g_Port });
    	listener->Start();
        // Run the I/O service on the requested number of threads
        std::vector<std::thread> v;
        v.reserve(config::g_Threads - 1);
        for (auto i = config::g_Threads - 1; i > 0; --i)
            v.emplace_back(
                [&ioc]
                {
                    ioc.run();
                });
        ioc.run();
        return EXIT_SUCCESS;
    }
}
