#include "Listener.h"

namespace shm::server
{
	Listener::Listener(net::io_context& ioc, tcp::endpoint endpoint)
		: m_IOContext(ioc), m_Acceptor(net::make_strand(ioc)) 
	{
		beast::error_code ec;

		m_Acceptor.open(endpoint.protocol(), ec);
		if(ec)
		{
			SHM_SV_ERR(ec);
			return;
		}

		m_Acceptor.set_option(net::socket_base::reuse_address(true), ec);
		if(ec)
		{
			SHM_SV_ERR(ec);
			return;
		}

		m_Acceptor.bind(endpoint, ec);
		if(ec)
		{
			SHM_SV_ERR(ec);
			return;
		}

		// Start listening for connections
		m_Acceptor.listen(net::socket_base::max_listen_connections, ec);
		if(ec)
		{
			SHM_SV_ERR(ec);
			return;
		}
	}

	void Listener::Start()
	{
		this->Accept();
	}

	void Listener::Accept()
	{
		m_Acceptor.async_accept(net::make_strand(m_IOContext),
			beast::bind_front_handler(&Listener::OnAccept, shared_from_this()));
	}

	void Listener::OnAccept(beast::error_code ec, tcp::socket socket)
	{
		if(ec)
			SHM_SV_ERR(ec);
		else
		{
			auto const& session = std::make_shared<Session>(std::move(socket));
			session->Start();
		}
		
		// Accept other connections
		this->Accept();
	}
}
