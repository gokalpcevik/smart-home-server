#include "Session.h"

namespace shm::server
{
	Session::Session(tcp::socket&& socket)
		: m_TCPStream(std::move(socket)), m_Lambda(*this)
	{


	}

	void Session::Start()
	{
		this->StartReading();
	}

	void Session::StartReading()
	{
		m_Request = {};

		m_TCPStream.expires_after(std::chrono::seconds(60LL));

		http::async_read(m_TCPStream, m_Buffer, m_Request,
			beast::bind_front_handler(
				&Session::OnRead,
				shared_from_this())
			);
	}

	void Session::Close()
	{
		// Send a TCP shutdown
		beast::error_code ec;
		m_TCPStream.socket().shutdown(tcp::socket::shutdown_send, ec);
		// At this point the connection is closed gracefully
	}

	void Session::OnRead(beast::error_code ec, std::size_t bytesTransferred)
	{
		boost::ignore_unused(bytesTransferred);

		// This means the connection was closed by the other side.
		if (ec == http::error::end_of_stream)
		{
			shm_trace("Connection was closed by the other side.");
			return Close();
		}

		if(ec)
		{
			SHM_SV_ERR(ec);
			return;
		}
		shm::server::HandleRequest(std::move(m_Request), m_Lambda);
	}

	void Session::OnWrite(bool close, beast::error_code ec, std::size_t bytesTransferred)
	{
		boost::ignore_unused(bytesTransferred);

		if (ec)
			return SHM_SV_ERR(ec);

		if (close)
		{
			// This means we should close the connection, usually because
			// the response indicated the "Connection: close" semantic.
			return Close();
		}

		// We're done with the response so delete it
		m_Response = nullptr;

		// Read another request
		this->StartReading();

	}
}
