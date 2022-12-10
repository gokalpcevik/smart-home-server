#include "Log.h"

namespace shm
{
	namespace Core
	{
		std::shared_ptr<spdlog::async_logger> Log::s_Logger;

		void Log::Init()
		{
			spdlog::init_thread_pool(8192, 2);

			std::vector<spdlog::sink_ptr> logSinks;
			logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
			logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("smart-home.log", true));

			logSinks[0]->set_pattern("%^[%T] %n: %v%$");
			logSinks[1]->set_pattern("[%T] [%l] %n: %v");

			s_Logger = std::make_shared<spdlog::async_logger>("smart-home", std::begin(logSinks), std::end(logSinks),spdlog::thread_pool());
			spdlog::register_logger(s_Logger);
			s_Logger->set_level(spdlog::level::trace);
			s_Logger->flush_on(spdlog::level::trace);
		}
	}
}
