#pragma once
#include <string_view>

namespace shm
{
	static std::string_view constexpr g_IpAddress = "0.0.0.0";
	static uint16_t constexpr g_Port = 9191U;
	static std::string_view constexpr g_DocumentRoot = ".";
}