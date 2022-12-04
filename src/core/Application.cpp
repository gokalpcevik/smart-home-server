#include "Application.h"


namespace shm::Core
{
    auto Application::Get() -> Application &
    {
        static Application instance;
        return instance;
    }

    auto Application::Start() -> int32_t
    {
        Log::Init();

        boost::asio::ip::address const address = net::ip::make_address(shm::g_IpAddress);

    	try
        {
            tcp::acceptor acceptor{ m_IOContext, {address, shm::g_Port} };
            tcp::socket socket{ m_IOContext };
            this->Serve(acceptor, socket);
            m_IOContext.run();
        }
    	catch(std::exception const& e)
        {
            smart_error("Error:{}", e.what());
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    void Application::Serve(tcp::acceptor& acceptor, tcp::socket& socket)
    {
        acceptor.async_accept(socket,
            [&](beast::error_code ec)
            {
                if (!ec)
                {
                    auto const connection = std::make_shared<server::Connection>(std::move(socket));
                    connection->Start();
                }
                this->Serve(acceptor, socket);
            });
    }
}
