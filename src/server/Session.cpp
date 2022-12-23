#include "Session.h"

namespace shm::server
{
	Session::Session(tcp::socket&& socket)
		: m_TCPStream(std::move(socket)), m_Lambda(*this)
	{


	}

	void Session::Start()
	{

		auto const& address = m_TCPStream.socket().remote_endpoint().address().to_string();
		shm_trace("Starting session with the remote TCP endpoint address of '{0}'", address);

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

		m_ReadPoint = std::chrono::steady_clock::now();
	}

	void Session::Close()
	{
		// Send a TCP shutdown
		beast::error_code ec;
		m_TCPStream.socket().shutdown(tcp::socket::shutdown_send, ec);

		if(ec)
			SHM_SV_ERR(ec);

		
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


		server::HandleRequest(std::move(m_Request), m_Lambda);
	}

	void Session::OnWrite(bool close, beast::error_code ec, std::size_t bytesTransferred)
	{
		boost::ignore_unused(bytesTransferred);

		if (ec)
			return SHM_SV_ERR(ec);

		m_WritePoint = std::chrono::steady_clock::now();

		auto const elapsed = m_WritePoint - m_ReadPoint;

		shm_info("{0} microseconds has elapsed between reading the request and writing the response.",
			std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count());

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
