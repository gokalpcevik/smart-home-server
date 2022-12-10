#pragma once

#include "ShmServerInclude.h"
#include "ErrorCode.h"
#include "../core/Log.h"
#include "Session.h"

namespace shm::server
{
	/**
	 * \brief Accepts the incoming connections and launches the HTTP sessions.
	 */
	class Listener : public std::enable_shared_from_this<Listener>
	{
	public:
		Listener(
			net::io_context& ioc,
			tcp::endpoint endpoint);

		Listener(Listener const&) = delete;
		Listener& operator=(Listener const&) = delete;

		void Start();

	private:
		void Accept();
		void OnAccept(beast::error_code ec, tcp::socket socket);


	private:
		net::io_context& m_IOContext;
		tcp::acceptor m_Acceptor;

	};

}