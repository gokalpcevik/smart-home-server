#pragma once
#define SPDLOG_WCHAR_TO_UTF8_SUPPORT

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace shm
{
    namespace Core
    {
        class Log
        {
        private:
            static std::shared_ptr<spdlog::async_logger> s_Logger;

        public:
            Log() = default;
            static void Init();
            static auto GetLogger() -> std::shared_ptr<spdlog::async_logger> & { return s_Logger; }
        };
    }
}

#define shm_trace(...) ::shm::Core::Log::GetLogger()->trace(__VA_ARGS__)
#define shm_debug(...) ::shm::Core::Log::GetLogger()->debug(__VA_ARGS__)
#define shm_info(...) ::shm::Core::Log::GetLogger()->info(__VA_ARGS__)
#define shm_warn(...) ::shm::Core::Log::GetLogger()->warn(__VA_ARGS__)
#define shm_error(...) ::shm::Core::Log::GetLogger()->error(__VA_ARGS__)
#define shm_critical(...) ::shm::Core::Log::GetLogger()->critical(__VA_ARGS__)