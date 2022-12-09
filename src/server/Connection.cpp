#include "Connection.h"

namespace shm::server
{

	Connection::Connection(boost::asio::ip::tcp::socket socket) : m_TcpSocket(std::move(socket))
	{

	}

	void Connection::Start()
	{
		this->ReadRequest();
		this->CheckDeadline();
	}

	void Connection::ReadRequest()
	{
		auto self = shared_from_this();

		http::async_read(
			m_TcpSocket,
			m_Buffer,
			m_Request,
			[self](beast::error_code ErrorCode,
			       std::size_t BytesTransferred)
			{
				boost::ignore_unused(BytesTransferred);
				if (!ErrorCode)
					self->ProcessRequest();
				else
					shm_error("Error while asyncronously reading HTTP message. \r\n Error:{0}", ErrorCode.what());
			});
	}

	void Connection::CheckDeadline()
	{
		auto self = shared_from_this();

		m_Deadline.async_wait(
			[self](beast::error_code ec)
			{
				if (!ec)
				{
					// Close socket to cancel any outstanding operation.
					self->m_TcpSocket.close(ec);
				}
			});
	}

	void Connection::ProcessRequest()
	{
		m_Response.version(m_Request.version());
		m_Response.keep_alive(false);

		switch (m_Request.method())
		{
		case http::verb::get:
			m_Response.result(http::status::ok);
			m_Response.set(http::field::server, "smart-home");
			this->CreateResponse();
			break;

		default:
			// We return responses indicating an error if
			// we do not recognize the request method.
			m_Response.result(http::status::bad_request);
			m_Response.set(http::field::content_type, "text/plain");
			beast::ostream(m_Response.body())
				<< "Invalid request-method '"
				<< std::string(m_Request.method_string())
				<< "'";
			break;
		}

		this->WriteResponse();
	}

	void Connection::CreateResponse()
	{
		if (m_Request.target() == "/time")
		{
			m_Response.set(http::field::content_type, "text/html");
			beast::ostream(m_Response.body())
				<< "<html>\n"
				<< "<head><title>Current time</title></head>\n"
				<< "<body>\n"
				<< "<h1>Current time </h1>\n"
				<< "<p>The current time is "
				<< std::time(0)
				<< " seconds since the epoch.</p>\n"
				<< "</body>\n"
				<< "</html>\n";
		}
		else if (m_Request.target() == "/light/status")
		{
			m_Response.set(http::field::content_type, "text/plain");
			beast::ostream(m_Response.body()) << "Light with ID=1 is on.\r\n";
		}
		else
		{
			m_Response.result(http::status::not_found);
			m_Response.set(http::field::content_type, "text/plain");
			beast::ostream(m_Response.body()) << "File not found\r\n";
		}
	}

	void Connection::WriteResponse()
	{
		auto self = shared_from_this();

		m_Response.content_length(m_Response.body().size());

		http::async_write(
			m_TcpSocket,
			m_Response,
			[self](beast::error_code ec, std::size_t)
			{
				self->m_TcpSocket.shutdown(tcp::socket::shutdown_send, ec);
				self->m_Deadline.cancel();
			});
	}
}
