#pragma once
#include <string_view>

namespace shm::config
{
	static std::string_view constexpr g_IpAddress = "0.0.0.0";
	static uint16_t constexpr g_Port = 52170U;
	static size_t constexpr g_Threads = 16ULL;
}