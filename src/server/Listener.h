#pragma once

#include "CommonServerIncludes.h"
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
			tcp::endpoint const& endpoint);

		Listener(Listener const&) = delete;
		Listener& operator=(Listener const&) = delete;

		/**
		 * \brief Starts accepting new connections.
		 */
		void Start();

	private:
		/**
		 * \brief Calls the http::async_accept function which returns immediately
		 * but initiates a function for the async operation.
		 */
		void Accept();
		/**
		 * \brief This is called when the member variable of type tcp::acceptor accepts a new connection. Always check the error code to see if it succeeded.
		 * \param ec 
		 * \param socket 
		 */
		void OnAccept(beast::error_code ec, tcp::socket socket);

	private:
		net::io_context& m_IOContext;
		tcp::acceptor m_Acceptor;

	};

}