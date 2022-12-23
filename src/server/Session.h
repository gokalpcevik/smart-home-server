#pragma once
#include "RequestHandler.h"
#include "CommonServerIncludes.h"
#include "../core/Log.h"
#include "ErrorCode.h"

namespace shm::server
{
	class Session : public std::enable_shared_from_this<Session>
	{
	public:
		Session(tcp::socket&& socket);
		Session(Session const&) = delete;
		Session& operator=(Session const&) = delete;


		void Start();
	private:
		void StartReading();
		void Close();

		void OnRead(beast::error_code ec, std::size_t bytesTransferred);
		void OnWrite(bool close, beast::error_code ec, std::size_t bytesTransferred);

	private:
		struct SendLambda
		{
			Session& r_self;

			explicit SendLambda(Session& self) : r_self(self)
			{
			}

			template <bool isRequest, class Body, class Fields>
			void operator()(http::message<isRequest, Body, Fields>&& msg) const
			{
				auto sp = std::make_shared<
					http::message<isRequest, Body, Fields>>(std::move(msg));
				r_self.m_Response = sp;

				http::async_write(
					r_self.m_TCPStream,
					*sp,
					beast::bind_front_handler(&Session::OnWrite,r_self.shared_from_this(), sp->need_eof()));
			}
		};

		std::chrono::time_point<std::chrono::steady_clock> m_ReadPoint;
		std::chrono::time_point<std::chrono::steady_clock> m_WritePoint;

		beast::tcp_stream m_TCPStream;
		beast::flat_buffer m_Buffer;
		http::request<http::string_body> m_Request;
		std::shared_ptr<void> m_Response;
		SendLambda m_Lambda;
	};
}
