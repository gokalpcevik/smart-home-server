#pragma once

#include <memory>
#include "CommonServerIncludes.h"
#include "../core/Log.h"

namespace shm::server
{
	class Connection : public std::enable_shared_from_this<Connection>
	{
	public:
		Connection(tcp::socket socket);

		void Start();

	private:

		void ReadRequest();
		void CheckDeadline();
		void ProcessRequest();
		void CreateResponse();
		void WriteResponse();

	private:

		tcp::socket m_TcpSocket;

		beast::flat_buffer m_Buffer{ 8192ULL };

		beast::http::request  <beast::http::dynamic_body> m_Request;
		beast::http::response <beast::http::dynamic_body> m_Response;

		beast::net::steady_timer m_Deadline{
			m_TcpSocket.get_executor(),
			std::chrono::seconds(60) };
	};
}
