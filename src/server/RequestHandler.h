#pragma once
#include "ShmServerInclude.h"
#include "../core/Log.h"
#include "ErrorCode.h"

namespace shm::server
{
    template<class Body, class Allocator>
    http::response<http::string_body> BadRequest(http::request<Body, http::basic_fields<Allocator>> const& request, std::string_view why)
    {
        http::response<http::string_body> res{ http::status::bad_request, request.version() };
        res.set(http::field::server, "smart-home-beast");
        res.set(http::field::content_type, "text/html");
        res.keep_alive(request.keep_alive());
        res.body() = std::string(why);
        res.prepare_payload();
        return std::move(res);
    }


    template<class Body, class Allocator>
    http::response<http::string_body> NotFound(http::request<Body, http::basic_fields<Allocator>> const& request, std::string_view target)
    {
        http::response<http::string_body> res{ http::status::not_found, request.version() };
        res.set(http::field::server, "smart-home-beast");
        res.set(http::field::content_type, "text/html");
        res.keep_alive(request.keep_alive());
        res.body() = fmt::format("Resource or target '{0}' was not found.", target);
        res.prepare_payload();
        return std::move(res);
    }

    template<class Body, class Allocator>
    http::response<http::string_body> InternalServerError(http::request<Body, http::basic_fields<Allocator>> const& request, std::string_view what)
    {
        http::response<http::string_body> res{ http::status::internal_server_error, request.version() };
        res.set(http::field::server, "smart-home-beast");
        res.set(http::field::content_type, "text/html");
        res.keep_alive(request.keep_alive());
        res.body() = fmt::format("An error has occurred: {0}", what);
        res.prepare_payload();
        return std::move(res);
    }

	template<class Body, class Allocator, class Send>
	void HandleRequest(http::request<Body, http::basic_fields<Allocator>>&& request,Send&& send)
	{
        switch(request.method())
        {
        default:
            return send(BadRequest(request, "Unknown HTTP-method"));

        case http::verb::head:
	        {
                http::response<http::empty_body> res{ http::status::ok, request.version() };
                res.set(http::field::server, "smart-home-beast");
                res.set(http::field::content_type, "text/html");
                res.keep_alive(request.keep_alive());
                return send(std::move(res));
	        }
        case http::verb::get:
	        {
	            if (request.target() == "/time")
	            {
	                http::response<http::dynamic_body> res{ http::status::ok, request.version() };
                    res.set(http::field::server, "smart-home-beast");
                    res.set(http::field::content_type, "text/html");
                    res.keep_alive(request.keep_alive());

                    beast::ostream(res.body())
	                    << "<html>\n"
	                    << "<head><title>Current time</title></head>\n"
	                    << "<body>\n"
	                    << "<h1>Current time </h1>\n"
	                    << "<p>The current time is "
	                    << std::time(0)
	                    << " seconds since the epoch.</p>\n"
	                    << "</body>\n"
	                    << "</html>\n";
	                return send(std::move(res));
	            }
                else
                {
                    return send(NotFound(request, request.target().to_string()));
                }
	        }

        }
	}
}