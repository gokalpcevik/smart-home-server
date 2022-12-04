#pragma once
#define SPDLOG_WCHAR_TO_UTF8_SUPPORT
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace shm
{
    namespace Core
    {
        class Log
        {
        private:
            static std::shared_ptr<spdlog::logger> s_Logger;

        public:
            Log() = default;
            static void Init();
            static auto GetLogger() -> std::shared_ptr<spdlog::logger> & { return s_Logger; }
        };
    }
}

#define smart_trace(...) ::shm::Core::Log::GetLogger()->trace(__VA_ARGS__)
#define smart_debug(...) ::shm::Core::Log::GetLogger()->debug(__VA_ARGS__)
#define smart_info(...) ::shm::Core::Log::GetLogger()->info(__VA_ARGS__)
#define smart_warn(...) ::shm::Core::Log::GetLogger()->warn(__VA_ARGS__)
#define smart_error(...) ::shm::Core::Log::GetLogger()->error(__VA_ARGS__)
#define smart_critical(...) ::shm::Core::Log::GetLogger()->critical(__VA_ARGS__)