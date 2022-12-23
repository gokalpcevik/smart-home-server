#pragma once
#include <fstream>
#include "CommonServerIncludes.h"
#include "shm_yaml.h"
#include "../core/Log.h"
#include "../embedded/SerialCommunication.h"
#include "../embedded/CommandBuilder.h"
#include <simdjson.h>
#include <yaml-cpp/yaml.h>
#include <ostream>
#include "ErrorCode.h"

namespace shm::server
{
    template<class Body, class Allocator>
    http::response<http::string_body> Ok(http::request<Body, http::basic_fields<Allocator>> const& request, std::string_view why)
    {
        http::response<http::string_body> res{ http::status::ok, request.version() };
        res.set(http::field::server, "smart-home-beast");
        res.set(http::field::content_type, "text/plain");
        res.keep_alive(request.keep_alive());
        res.body() = std::string(why);
        res.prepare_payload();
        return std::move(res);
    }

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
        embedded::CommandBuilder cmdBuilder;
        embedded::SerialCommunication comm("\\\\.\\COM8", embedded::BaudRate::BR115200);

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
                    /*
                     *  This endpoint expects a request body like below:
                     *  {
                     *      "room": "livingroom" or "bedroom",
                     *      "func": "brightness" or "power" or "window" or "smartlight" or "sunlight" or "changeColor"
                     *      "targetValue": int64 0-100 or true/false or true/false or true/false or true/false or "red"/"turquoise"/"green"
                     *  }
                     *
                     *
                     *  Response body is like below:
                     *  If the function is executed correctly on Arduino:
                     *  {
                     *      "response":true
                     *  }
                     *
                     *  If the function is not executed correctly on Arduino:
                     *  {
                     *      "response":false
                     *  }
                     */


                    // Embedded/board communication goes here
                    std::string json = request.body();
                    simdjson::dom::parser parser;
                    auto const requestData = parser.parse(json);
                    
                    if(requestData.error() != simdjson::SUCCESS)
                    {
                        return send(BadRequest(request, "Invalid JSON data."));
                    }

	                auto room = requestData["room"].get_string();
	                auto command = requestData["func"].get_string();
	                auto targetValue = requestData["targetValue"];

	                if (room.error()        != simdjson::SUCCESS ||
						command.error()     != simdjson::SUCCESS ||
						targetValue.error() != simdjson::SUCCESS)
                    {
                        return send(BadRequest(request, "Invalid JSON key-value pair at either 'room','func','targetValue'"));
                    }


                    embedded::ROOM roomEnum = embedded::ROOM::Invalid;

                    if (room.value() == "livingroom") roomEnum = embedded::ROOM::LivingRoom;
                    else if (room.value() == "bedroom") roomEnum = embedded::ROOM::Bedroom;
                    else return send(BadRequest(request, "{\"message\":\"Room not found\"}"));

                    if(command.value() == "power")
                    {
                        if(targetValue.get_bool().error() != simdjson::SUCCESS)
                            return send(
                                BadRequest(request, 
                                fmt::format("'targetValue' expect a value of type 'bool' but received the value '{0}'", targetValue.value())));


                    	auto power = targetValue.get_bool().value();
                        uint64_t boardCmd = cmdBuilder.BuildPower(roomEnum, power);
                    	bool success = comm.Write(&boardCmd, 8);
                        auto responseStr = fmt::format("{{ \"success\":{0} }}", success);

                        return send(Ok(request, responseStr));

                    }
                    else if (command.value() == "window")
                    {
                        if (targetValue.get_bool().error() != simdjson::SUCCESS)
                            return send(
                                BadRequest(request,
                                    fmt::format("'targetValue' expect a value of type 'bool' but received the value '{0}'", targetValue.value())));

                        auto doorOn = targetValue.get_bool().value();
                        uint64_t boardCmd = cmdBuilder.BuildWindow(doorOn);
                        bool success = comm.Write(&boardCmd, 8);
                        auto responseStr = fmt::format("{{ \"response\":{0} }}", success);

                        
                        return send(Ok(request,responseStr));
                    }
                    else if(command.value() == "smartlight")
                    {
                        if (targetValue.get_bool().error() != simdjson::SUCCESS)
                            return send(
                                BadRequest(request,
                                    fmt::format("'targetValue' expect a value of type 'bool' but received the value '{0}'", targetValue.value())));


                        auto smartlightOn = targetValue.get_bool().value();
                        uint64_t boardCmd = cmdBuilder.BuildSmartLight(smartlightOn);
                        bool success = comm.Write(&boardCmd, 8);
                        auto responseStr = fmt::format("{{ \"response\":{0} }}", success);

                    	return send(Ok(request,responseStr));
                    }
                    else if(command.value() == "sunlight")
                    {
                        if (targetValue.get_bool().error() != simdjson::SUCCESS)
                            return send(
                                BadRequest(request,
                                    fmt::format("'targetValue' expect a value of type 'bool' but received the value '{0}'", targetValue.value())));


                        auto sunlightOn = targetValue.get_bool().value();
                        uint64_t boardCmd = cmdBuilder.BuildSunlight(sunlightOn);
                        bool success = comm.Write(&boardCmd, 8);
                        auto responseStr = fmt::format("{{ \"response\":{0} }}", success);

                        return send(Ok(request,responseStr));
                    }
                    else if(command.value() == "brightness")
                    {
	                    if (targetValue.get_int64().error() != simdjson::SUCCESS)
	                        return send(
	                            BadRequest(request,
	                                fmt::format("'targetValue' expect a value of type 'int/uint' but received the value '{0}'", targetValue.value())));

						
						shm_trace("Brightness value: {}",targetValue.get_int64().value());

                        auto brightness = targetValue.get_int64().value();
                        uint64_t boardCmd = cmdBuilder.BuildBrightness(roomEnum,brightness);
                        bool success = comm.Write(&boardCmd, 8);

                        auto responseStr = fmt::format("{{ \"response\":{0} }}", success);

	                    return send(Ok(request,responseStr));
                    }
                    else if(command.value() == "selectColor")
                    {
                        if (targetValue.get_string().error() != simdjson::SUCCESS)
                            return send(
                                BadRequest(request,
                                    fmt::format("'targetValue' expect a value of type 'string' but received the value '{0}'", targetValue.value())));


                    }
                    // If the func is invalid
                    else
                    {
	                   return send(BadRequest(request, "{ \"response\":\"Invalid function.\" }"));
                    }
				}

                else if (request.target() == "/door")
                {
					/*
					 *  This endpoint expects a request body like below:
					 *  {
					 *      "password": int (6 digits) ,
					 *      "targetValue":true,false // Open or close the door
					 *  }
					 *
					 *
					 *  Response body is like below:
					 *  If the password is correct:
					 *  {
					 *      "response":"Door opened."
					 *  }
					 *
					 *  If the password is not correct:
					 *  {
					 *      "response":"Invalid password."
					 *  }
					 */

					std::string json = request.body();
					simdjson::dom::parser parser;
					auto const requestData = parser.parse(json);

                    if (requestData.error() != simdjson::SUCCESS)
                    {
                        return send(BadRequest(request, "{ \"response\":\"Invalid JSON data.\" }"));
                    }

                    auto file = YAML::LoadFile("pass.yml");
                    YAML::Node root = file["Server"];
                    YAML::Node passwordNode = root["Password"];

                    auto serverPassword = shm::yaml::AsIf<std::int64_t>(passwordNode);
                    auto enteredPassword = requestData["password"].get_int64();

                    if(enteredPassword.error() != simdjson::SUCCESS || !serverPassword)
                    {
                        return send(InternalServerError(request, 
                            "Either the entered password or the password stored on the server is not of the type 'int'."));
                    }

                    if (serverPassword.value() == enteredPassword.value())
                    {
                        auto targetValue = requestData["targetValue"].get_bool();

                        if(targetValue.error() != simdjson::SUCCESS)
                        {
                            return send(BadRequest(request, "{ \"response\":\"Invalid JSON data on the key-value pair 'targetValue'.\" }"));
                        }

                        uint64_t boardCmd = cmdBuilder.BuildDoor(targetValue.value());
                        bool success = comm.Write(&boardCmd, 8);
                        auto responseStr = fmt::format("{{ \"response\":\"Door state changed successfully.\" }}");
                        shm_info("Door state has been changed to '{0}'.",targetValue.value());

                    	return send(Ok(request,responseStr));
                    }
                    else
                    {
                        return send(BadRequest(request, "{ \"response\":\"Invalid password.\" }"));

                    }
                }

                else if(request.target() == "/password")
                {
	                /*
	                 *  This endpoint expects a request body like below:
	                 *  {
	                 *      "oldPassword": int (6 digits),
	                 *      "newPassword": int (6 digits)
	                 *  }
	                 *
	                 *
	                 *  Response body is like below:
	                 *  If the old password is correct:
	                 *  {
	                 *      "response":"Password changed."
	                 *  }
	                 *
	                 *  If the password is not correct:
	                 *  {
	                 *      "response":"Invalid password."
	                 *  }
	                 */


	                std::string json = request.body();
	                simdjson::dom::parser parser;
	                auto const requestData = parser.parse(json);

	                if (requestData.error() != simdjson::SUCCESS)
	                {
	                    return send(BadRequest(request, "{ \"response\":\"Invalid JSON data.\" }"));
	                }

                    auto oldPassword = requestData["oldPassword"].get_int64();
	                auto newPassword = requestData["newPassword"].get_int64();

	                if (oldPassword.error() != simdjson::SUCCESS ||
	                    newPassword.error() != simdjson::SUCCESS)
	                {
                        shm_warn("Invalid password attempt.");
	                    return send(BadRequest(request, "{ \"response\":\"Invalid JSON data or type.\" }"));
	                }

                    YAML::Node file;
	                try
                    {
                        file = YAML::LoadFile("pass.yml");
                    }
                    catch (YAML::BadFile const& e)
                    {
                        shm_error("Either the password file does not exist or it was corrupted.");
                        shm_info("Creating a new password file.");
                        YAML::Emitter emitter;
                        emitter << YAML::BeginMap;
	                        emitter << YAML::Key << "Server";
	                        emitter << YAML::Value;
	                        emitter << YAML::BeginMap;
		                        emitter << YAML::Key << "Password";
		                        emitter << YAML::Value << 111111;
	                        emitter << YAML::EndMap;
                        emitter << YAML::EndMap;

                        std::ofstream fon("pass.yml");
                        fon.write(emitter.c_str(), emitter.size());
                        fon.close();
                    }
                    file = YAML::LoadFile("pass.yml");
	                YAML::Node root = file["Server"];
                    YAML::Node passwordNode = root["Password"];

                    auto password = shm::yaml::AsIf<std::int64_t>(passwordNode);

                    if(!password)
                    {
                        return send(InternalServerError(request, "Password stored on the server is not of the type 'int'."));
                    }

                    if(password.value() != oldPassword.value())
                    {
                        return send(BadRequest(request, "{ \"response\":\"Invalid old password.\" }"));
                    }

                    shm_warn("Door password has been changed.");

                    YAML::Emitter emitter;
                    emitter << YAML::BeginMap;
                        emitter << YAML::Key << "Server";
                        emitter << YAML::Value;
                        emitter << YAML::BeginMap;
                            emitter << YAML::Key << "Password";
                            emitter << YAML::Value << newPassword.value();
                            emitter << YAML::EndMap;
                        emitter << YAML::EndMap;

                    std::ofstream fon("pass.yml");
                    fon.write(emitter.c_str(),emitter.size());
                    fon.close();

                    auto responseStr = fmt::format("{{ \"response\":\"Password changed successfully.\" }}");
                    return send(Ok(request,responseStr));
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