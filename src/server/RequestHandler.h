#pragma once
#include "CommonServerIncludes.h"
#include "../core/Log.h"
#include "../embedded/SerialCommunication.h"
#include "../embedded/CommandBuilder.h"
#include <simdjson.h>
#include "ErrorCode.h"

namespace shm::server
{
    template<class Body, class Allocator>
    http::response<http::string_body> BadRequest(http::request<Body, http::basic_fields<Allocator>> const& request, std::string_view why)
    {
        http::response<http::string_body> res{ http::status::bad_request, request.version() };
        res.set(http::field::server, "smart-home-beast");
        res.set(http::field::content_type, "text/plain");
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
        case http::verb::options:
	        {
	            http::response<http::empty_body> res{ http::status::ok, request.version() };
	            res.set(http::field::server, "smart-home-beast");
	            res.set(http::field::content_type, "text/html");
                res.set(http::field::access_control_allow_origin, "*");
	            res.keep_alive(request.keep_alive());
	            return send(std::move(res));
	        }
        case http::verb::post:
	        {
				if(request.target() == "/room")
				{
                    // Embedded/board communication goes here
                    std::string json = request.body();
                    simdjson::dom::parser parser;
                    auto const requestData = parser.parse(json);

                    

                    if(requestData.error() != simdjson::SUCCESS)
                    {
                        return send(BadRequest(request, "Invalid JSON data."));
                    }

                    embedded::CommandBuilder cmdBuilder;
	                embedded::SerialCommunication comm("\\\\.\\COM3", embedded::BaudRate::BR115200);

	                auto room = requestData["room"].get_string();
	                auto command = requestData["func"].get_string();
	                auto targetValue = requestData["targetValue"];

	                if (room.error()        != simdjson::SUCCESS ||
						command.error()     != simdjson::SUCCESS ||
						targetValue.error() != simdjson::SUCCESS)
                    {
                        return send(BadRequest(request, "Invalid JSON key-value pair at either 'room','func','targetValue'"));
                    }


                    embedded::Room roomEnum = embedded::Room::Invalid;

                    if (room.value() == "livingroom") roomEnum = embedded::Room::LivingRoom;
                    else if (room.value() == "bedroom") roomEnum = embedded::Room::Bedroom;
                    else return send(BadRequest(request, "{\"message\":\"Room not found\"}"));

                    if(command.value() == "power")
                    {
                        if(targetValue.get_bool().error() != simdjson::SUCCESS)
                            return send(
                                BadRequest(request, 
                                fmt::format("'targetValue' expect a value of type 'bool' but received the value '{0}'", targetValue.value())));


                    	auto power = targetValue.get_bool().value();
                        uint64_t boardCmd = cmdBuilder.BuildPowerCmd(roomEnum, power);
                    	bool success = comm.Write(&boardCmd, 8);

                    	http::response<http::string_body> res{ http::status::ok, request.version() };
                        res.set(http::field::server, "smart-home-beast");
                        res.set(http::field::content_type, "text/plain");
                        res.keep_alive(request.keep_alive());
                        res.body() = request.body();
                    	res.prepare_payload();

                        return send(std::move(res));

                    }
                    else
                    {
                        send(BadRequest(request, "Invalid home function."));
                    }
                    // Send the response back to app
                    return send(BadRequest(request,"Unknown error."));
				}

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